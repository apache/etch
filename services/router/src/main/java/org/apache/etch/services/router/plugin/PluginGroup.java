/* $Id$
 *
 * Copyright 2009-2010 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
package org.apache.etch.services.router.plugin;

import java.util.ArrayList;
import java.util.List;

import org.apache.etch.services.router.ApplicationConnectionInfo;
import org.apache.etch.services.router.ConnectionStackInfo;
import org.apache.etch.services.router.EtchRouterManager;
import org.apache.etch.services.router.PluginInstallInfo;
import org.apache.etch.services.router.EtchRouter.EtchRouterException;
import org.apache.etch.services.router.utils.ApplicationAttributes;
import org.apache.etch.services.router.utils.PluginAttributes;


/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public abstract class PluginGroup
{
	/**
	 * 
	 */
	public static final String _URL_PROPERTY_KEY_PREFIX = "plugin.member.url.";
	
	/**
	 * 
	 */
	public static final String _META_DATA_PROPERTY_KEY_PREFIX = "plugin.member.metadata.";
	
	/**
	 * Factory method to new a PluginGroup of a particular type
	 * 
	 * @param name
	 * @param routerMgr
	 * @param installInfo
	 * @return
	 */
	public static PluginGroup newPluginGroup( String name, EtchRouterManager routerMgr, PluginInstallInfo installInfo)
	{
		String groupType = installInfo.getMetaProperty( "plugin.group.type" );
		PluginGroup group = null;
		if ("roundrobin".equalsIgnoreCase( groupType ))
		{
			group = new RoundRobinPluginGroup(name, routerMgr, installInfo);
		}
		if (group==null)
			group = new FailoverPluginGroup(name, routerMgr, installInfo);
		return group;
	}
	
	protected String _name = null;
	
	protected List<PluginMember> _pluginMembers = null;
	
	protected PluginInstallInfo _installInfo = null;
	
	protected EtchRouterManager _etchRouterManager = null;
	
	protected List<ApplicationConnectionInfo> _appConnections = null;
	
	protected PluginGroup(String name, EtchRouterManager routerMgr, PluginInstallInfo installInfo)
	{
		_name = name;
		_pluginMembers = new ArrayList<PluginMember>();
		_etchRouterManager = routerMgr;
		_installInfo = installInfo;
		_appConnections = new ArrayList<ApplicationConnectionInfo>();
		populatePluginMembers();
	}
	
	private void populatePluginMembers()
	{
		String[] urlKeys = _installInfo.getSortedMetaPropertyKeysWithPrefix( _URL_PROPERTY_KEY_PREFIX );
		for (String key : urlKeys)
		{
			String url = _installInfo.getMetaProperty( key );
			String suffix = key.substring( _URL_PROPERTY_KEY_PREFIX.length() );
			String metaData = _installInfo.getMetaProperty( String.format( "%s%s", _META_DATA_PROPERTY_KEY_PREFIX, suffix ) );
			PluginMember member = new PluginMember(this, url, metaData);
			_pluginMembers.add( member );
		}
	}
	
	/**
	 * Getter
	 * 
	 * @return
	 */
	public String getName()
	{
		return _name;
	}
	
	/**
	 * 
	 * @return
	 * @throws EtchRouterException
	 */
	public synchronized PluginMember getActiveMember( ApplicationAttributes appAttrs ) throws EtchRouterException
	{
		if (_pluginMembers.isEmpty())
			throw new EtchRouterException(1, String.format( "No member is registered in plugin group '%s'", getName()));
		for (PluginMember member : _pluginMembers)
		{
			if (member.isActive() && member.getMetaAttributes().matches( appAttrs ))
				return member;
		}
		throw new EtchRouterException(1, String.format( "No active member is found in plugin group '%s' that matches application attributes \"%s\"", getName(), appAttrs.getEncodedString()));
	}
	
	/**
	 * 
	 * @param appConnection
	 */
	public synchronized void addApplicationConnection( ApplicationConnectionInfo appConnection )
	{
		_appConnections.add( appConnection );
	}
	
	/**
	 * 
	 */
	public void refreshAppConnections()
	{
		for (ApplicationConnectionInfo appConnection : _appConnections)
		{
			appConnection.initPluginConnectionIfNotConnected( this );
		}
	}
	
	/**
	 * 
	 * @param appConnection
	 */
	public synchronized void removeApplicationConnection( ApplicationConnectionInfo appConnection )
	{
		_appConnections.remove( appConnection );
	}
	
	/**
	 * 
	 * @param connInfo
	 * @throws EtchRouterException
	 */
	public void onPluginMemberConnectionDown( ConnectionStackInfo connInfo ) throws EtchRouterException
	{
		if (connInfo==null) return;
		for (PluginMember member : _pluginMembers)
		{
			if (connInfo==member.getConnInfo())
			{
				member.onRemoteConnectionDown( );
			}
		}
	}

	/**
	 * 
	 * @return
	 */
	public PluginInstallInfo getInstallInfo()
	{
		return _installInfo;
	}
	
	public List<PluginMember> getPluginMembers()
	{
		List<PluginMember> list = new ArrayList<PluginMember>();
		list.addAll( _pluginMembers );
		return list;
	}
	
	protected List<PluginMember> getActivePluginMembers( ApplicationAttributes appAttrs )
	{
		List<PluginMember> list = new ArrayList<PluginMember>(_pluginMembers.size());
		for (PluginMember member : _pluginMembers)
		{
			if (member.isActive()) {
				PluginAttributes pluginAttrs = member.getMetaAttributes();
				if (pluginAttrs.matches( appAttrs ))
					list.add(member);
			}
		}
		return list;
	}
}
