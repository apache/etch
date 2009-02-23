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

package org.apache.etch.services.ns;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.services.ns.NameServiceHelper;
import org.apache.etch.services.ns.NameServiceServer;
import org.apache.etch.services.ns.RemoteNameServiceClient;
import org.apache.etch.util.core.io.Transport;


/**
 * Main program for NameServiceServer. This program makes a listener to accept
 * connections from MainNameServiceClient.
 */
public class MainNameServiceListener implements NameServiceHelper.NameServiceServerFactory
{
	/**
	 * Main program for NameServiceServer.
	 * 
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		// TODO Change to correct URI
		String uri = "tcp://0.0.0.0:8002";
		
		ServerFactory listener = NameServiceHelper.newListener( uri, null,
			new MainNameServiceListener() );

		// Start the Listener
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		System.out.println( "Naming service UP at : " + uri );
		System.in.read();
	}

	public NameServiceServer newNameServiceServer( RemoteNameServiceClient client )
	{
		return new ImplNameServiceServer( client );
	}
}
