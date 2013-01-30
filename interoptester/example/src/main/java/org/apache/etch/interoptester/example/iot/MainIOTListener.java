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

package org.apache.etch.interoptester.example.iot;


import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.interoptester.example.iot.IOTHelper;
import org.apache.etch.interoptester.example.iot.IOTServer;
import org.apache.etch.interoptester.example.iot.RemoteIOTClient;
import org.apache.etch.util.core.io.Transport;


/**
 * Main program for IOTServer. This program makes a listener to accept
 * connections from MainIOTClient.
 */
public class MainIOTListener implements IOTHelper.IOTServerFactory
{
	/**
	 * Main program for IOTServer.
	 * 
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		if (args.length > 1)
		{
			System.err.println( "usage: MainIOTClient [uri]" );
			System.exit( 1 );
		}
		
		String uri = args.length > 0 ? args[0] : "tcp://localhost:4001";
		
		ServerFactory listener = IOTHelper.newListener( uri, null,
			new MainIOTListener() );

		// Start the Listener
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		System.out.println( "listener started: "+listener );
	}

	public IOTServer newIOTServer( RemoteIOTClient client )
	{
		return new ImplIOTServer( client );
	}
}
