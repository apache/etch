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


/**
 * Main implementation for ClosingClient.
 */
public class MainClosingClient implements ClosingHelper.ClosingClientFactory
{
	/**
	 * Main for ClosingClient
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args)
		throws Exception
	{
		Log.addSink( null );
		
		Log.report( "MainClosingClient.startingListener" );
		MainClosingListener.main( new String[] { "tcp://localhost:0" } );
		String uri = MainClosingListener.getLocalUri();
		Log.report( "MainClosingClient.listenerUp", "uri", uri );
		
		RemoteClosingServer server = ClosingHelper.newServer( uri, null, new MainClosingClient() );
		
		Log.report( "MainClosingClient.startingServer" );
		server._startAndWaitUp( 4000 );
		Log.report( "MainClosingClient.serverUp" );
		
		Thread.sleep( 100 );
		
		// this sends a message to the server which then closes the connection.
		
		Log.report( "MainClosingClient.sayingCloseToServer" );
		server.close();
		Log.report( "MainClosingClient.saidCloseToServer" );
		
		Thread.sleep( 100 );
		
		// waiting for server to close.
		
		Log.report( "MainClosingClient.waitingServerDown" );
		server._waitDown( 4000 );
		Log.report( "MainClosingClient.serverDown" );
	}

	public ClosingClient newClosingClient( RemoteClosingServer server )
		throws Exception
	{
		return new ImplClosingClient( server );
	}
}
