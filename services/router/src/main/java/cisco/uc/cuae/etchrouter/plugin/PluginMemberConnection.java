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

import cisco.uc.cuae.etchrouter.ERRemoteServer;

public class PluginMemberConnection
{

	private PluginMember _member = null;
	
	private ERRemoteServer _remoteServer = null;
	
	public PluginMemberConnection( PluginMember member, ERRemoteServer remoteServer )
	{
		_member = member;
		_remoteServer = remoteServer;
	}

	public PluginMember getMember()
	{
		return _member;
	}

	public ERRemoteServer getRemoteServer()
	{
		return _remoteServer;
	}

	@Override
	public String toString()
	{
		return String.format( "%s[member:%s, remote-server:%s]", this.getClass().getSimpleName(), _member, _remoteServer );
	}
	
	
	
}
