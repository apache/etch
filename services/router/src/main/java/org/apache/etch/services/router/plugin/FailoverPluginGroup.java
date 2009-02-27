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

import org.apache.etch.services.router.ConnectionStackInfo;
import org.apache.etch.services.router.EtchRouterManager;
import org.apache.etch.services.router.PluginInstallInfo;
import org.apache.etch.services.router.EtchRouter.EtchRouterException;
import org.apache.etch.services.router.utils.ApplicationAttributes;


/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class FailoverPluginGroup extends PluginGroup
{
	
	private List<PluginMember> _primaryMembers = new ArrayList<PluginMember>();
	/**
	 * 
	 * @param name
	 * @param routerMgr
	 * @param installInfo
	 */
	public FailoverPluginGroup( String name, EtchRouterManager routerMgr, PluginInstallInfo installInfo)
	{
		super( name, routerMgr, installInfo);
	}

	@Override
	public synchronized PluginMember getActiveMember(
		ApplicationAttributes appAttrs ) throws EtchRouterException
	{
		for (PluginMember aMember : _primaryMembers)
		{
			if (aMember.isActive() && aMember.getMetaAttributes().matches( appAttrs ))
				return aMember;
		}
		PluginMember member = super.getActiveMember( appAttrs );
		_primaryMembers.add( member );
		return member;
	}

	@Override
	public synchronized void onPluginMemberConnectionDown( ConnectionStackInfo connInfo )
		throws EtchRouterException
	{
		super.onPluginMemberConnectionDown( connInfo );
		for (PluginMember member : _primaryMembers)
		{
			if (connInfo==member.getConnInfo())
			{
				_primaryMembers.remove( member );
				return;
			}
		}
	}
	
	
}
