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
package org.apache.etch.services.router;

import org.apache.etch.bindings.java.support.ObjSession;

public class ImplClient extends ImplBase implements ObjSession
{
	private ERRemoteServer _server = null;
	
	public ImplClient(ERRemoteServer server, EtchRouterManager etchRouterManager)
	{
		super(etchRouterManager);
		_server = server;
	}

	public void _sessionControl( Object control, Object value )
		throws Exception
	{
		// TODO Auto-generated method stub
		
	}

	public void _sessionNotify( Object event ) throws Exception
	{
		// TODO Auto-generated method stub
		
	}

	public Object _sessionQuery( Object query ) throws Exception
	{
		// TODO Auto-generated method stub
		return null;
	}
	
	
}
