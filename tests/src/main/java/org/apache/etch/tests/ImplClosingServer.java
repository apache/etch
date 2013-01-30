/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.tests;

import org.apache.etch.util.Log;
import org.apache.etch.util.core.io.TcpTransport;


/**
 * Call to message translator for ClosingServer.
 */
public class ImplClosingServer extends BaseClosingServer
{
	/**
	 * Constructs the ImplClosingServer.
	 *
	 * @param client
	 */
	public ImplClosingServer( RemoteClosingClient client )
	{
		this.client = client;
	}
	
	private final RemoteClosingClient client;

	@Override
	public void close()
	{
		try
		{
			Log.report( "ImplClosingServer.sayingStopToClient" );
			client._transportControl( TcpTransport.STOP, null );
			Log.report( "ImplClosingServer.saidStopToClient" );
		}
		catch ( Exception e )
		{
			throw new RuntimeException( "caught exception", e );
		}
	}

	@Override
	public void _sessionNotify( Object event ) throws Exception
	{
		Log.report( "ImplClosingServer._sessionNotify", "event", event );
	}
}
