/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
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

package etch.tests;

import etch.util.Log;

/**
 * Call to message translator for ClosingClient.
 */
public class ImplClosingClient extends BaseClosingClient
{
	/**
	 * Constructs the ImplClosingClient.
	 * @param server 
 	 */
	public ImplClosingClient( RemoteClosingServer server )
	{
		this.server = server;
	}
	
	@SuppressWarnings( "unused" )
	private final RemoteClosingServer server;

	public void _sessionNotify( Object event ) throws Exception
	{
		Log.report( "ImplClosingClient._sessionNotify", "event", event );
	}
}
