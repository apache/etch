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
package cisco.uc.cuae.etchrouter.plugin;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.logging.Level;
import java.util.logging.Logger;

import cisco.uc.cuae.etchrouter.ERRemoteServer;
import cisco.uc.cuae.etchrouter.EtchRouterManager;
import cisco.uc.cuae.etchrouter.EtchRouterManagerHelper;
import cisco.uc.cuae.etchrouter.ConnectionStackInfo;
import cisco.uc.cuae.etchrouter.utils.PluginAttributes;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class PluginMember
{
	private static final Logger _LOGGER = Logger.getLogger( PluginMember.class.getName());
	
	private PluginGroup _pluginGroup = null;
	
	private String _url = null;
	private String _metaData = null;
	private PluginAttributes _metaAttrs = null;
	private List<PluginMemberConnection> _memberConnections = null;
	
	private ERRemoteServer _remoteServer = null;
	
	private boolean _active = false;
	
	/**
	 * 
	 * @param pluginGroup
	 * @param url
	 * @param metaData
	 */
	public PluginMember( PluginGroup pluginGroup, String url, String metaData )
	{
		_pluginGroup = pluginGroup;
		_url = url;
		_metaData = metaData;
		_memberConnections = Collections.synchronizedList( new ArrayList<PluginMemberConnection>());
		try
		{
			_metaAttrs = new PluginAttributes(_pluginGroup.getName(), _metaData);
		}
		catch (Exception e)
		{
			String msg = String.format( "Invalid plugin member metaData \"%s\"", _metaData);
			_LOGGER.log( Level.SEVERE, msg, e );
			try
			{
				_metaAttrs = new PluginAttributes(_pluginGroup.getName(), null);
			}
			catch (Exception ee)
			{
				_LOGGER.log( Level.SEVERE, "Got exception with NULL plugin member metaData: ", e );
			}
		}
	}

	public PluginAttributes getMetaAttributes()
	{
		return _metaAttrs;
	}
	
	public PluginGroup getPluginGroup()
	{
		return _pluginGroup;
	}
	
	public String getUrl()
	{
		return _url;
	}

	public String getMetaData()
	{
		return _metaData;
	}
	
	public ConnectionStackInfo getConnInfo()
	{
		return (_remoteServer==null ? null : _remoteServer.getConnectionStackInfo());
	}
	
	public void addMemberConnection(PluginMemberConnection memberConn)
	{
		_memberConnections.add( memberConn );
	}
	
	public void removeMemberConnection(PluginMemberConnection memberConn)
	{
		_memberConnections.remove( memberConn );
	}
	
	public int sizeOfMemberConnections()
	{
		return _memberConnections.size();
	}

	public synchronized void checkState(EtchRouterManager etchRouterManager)
	{
		if (_active) return;
		ConnectionStackInfo connInfo = null;
		try
		{
			_remoteServer = EtchRouterManagerHelper.newServer( etchRouterManager, _url, null, _pluginGroup.getName() );
			connInfo = _remoteServer.getConnectionStackInfo();
			connInfo.setConnectionType( ConnectionStackInfo.ConnectionType.PLUGIN_MONITOR_CONN, _pluginGroup.getName() );
			_remoteServer._startAndWaitUp( etchRouterManager.getConnectionStartMaxDelay() );
			_active = true;
			etchRouterManager.addConnectionStackInfo( connInfo );
			_LOGGER.log( Level.INFO, "Succefully created a monitor connection to plugin service url \"{0}\", the monitor connection is \"{1}\"", new Object[]{_url, _remoteServer.toString()} );
		}
		catch (Exception e)
		{
			_LOGGER.log( Level.FINER, String.format( "Failed to connection to plugin service url \"%s\"", _url), e );
			_remoteServer = null;
			if (connInfo != null) etchRouterManager.removeConnectionStackInfo( connInfo );
		}
	}
	
	public synchronized void onRemoteConnectionDown()
	{
		_active = false;
		if (_remoteServer!=null)
		{
			_remoteServer = null;
		}
	}
	
	public synchronized boolean isActive()
	{
		return _active;
	}
	
	@Override
	public String toString()
	{
		return String.format( "%s[url:%s, meta-data:%s]", this.getClass().getSimpleName(), _url, _metaData );
	}
	
	
}