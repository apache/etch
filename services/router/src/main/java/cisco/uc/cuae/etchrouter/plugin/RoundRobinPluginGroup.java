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

import java.util.List;

import cisco.uc.cuae.etchrouter.EtchRouterManager;
import cisco.uc.cuae.etchrouter.PluginInstallInfo;
import cisco.uc.cuae.etchrouter.EtchRouter.EtchRouterException;
import cisco.uc.cuae.etchrouter.utils.ApplicationAttributes;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class RoundRobinPluginGroup extends PluginGroup
{
	
	/**
	 * 
	 * @param name
	 * @param routerMgr
	 * @param installInfo
	 */
	public RoundRobinPluginGroup( String name, EtchRouterManager routerMgr, PluginInstallInfo installInfo)
	{
		super( name, routerMgr, installInfo);
	}

	@Override
	public synchronized PluginMember getActiveMember( ApplicationAttributes appAttrs ) throws EtchRouterException
	{
		List<PluginMember> list = getActivePluginMembers( appAttrs );
		PluginMember memberFound = null;
		int sizeFound = -1;
		// find the first instance that has the smallest size of instance connections:
		for (PluginMember memberInfo : list)
		{
			if (memberFound==null)
			{
				memberFound = memberInfo;
				sizeFound = memberFound.sizeOfMemberConnections();
			}
			else
			{
				int thisSize = memberInfo.sizeOfMemberConnections();
				if (thisSize<sizeFound)
				{
					memberFound = memberInfo;
					sizeFound = thisSize;
				}
			}
		}
		if (memberFound==null)
			throw new EtchRouterException(1, String.format( "No matching active member is registered in round robin plugin group \"%s\", application attributes is \"%s\"",
				_installInfo.getName(), appAttrs.getEncodedString()));
		return memberFound;
	}

}
