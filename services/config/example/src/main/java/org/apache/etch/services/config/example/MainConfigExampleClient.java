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

package org.apache.etch.services.config.example;

/**
 * Main program for ConfigExampleClient. This program makes a connection to the
 * listener created by MainConfigExampleListener.
 */
public class MainConfigExampleClient implements ConfigExampleHelper.ConfigExampleClientFactory
{
	/**
	 * Main program for ConfigExampleClient.
	 * 
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		// TODO Change to correct URI
		String uri = "tcp://localhost:4002";
		
		RemoteConfigExampleServer server = ConfigExampleHelper.newServer( uri, null,
			new MainConfigExampleClient() );

		// Connect to the service
		server._startAndWaitUp( 4000 );

		Integer z = server.add( 4, 5 );
		System.out.println( "add 4, 5 => "+z );

		// Disconnect from the service
		server._stopAndWaitDown( 4000 );
	}

	public ConfigExampleClient newConfigExampleClient( RemoteConfigExampleServer server )
		throws Exception
	{
		return new ImplConfigExampleClient( server );
	}
}
