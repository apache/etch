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

import org.apache.etch.services.ns.NameServiceClient;
import org.apache.etch.services.ns.NameServiceHelper;
import org.apache.etch.services.ns.RemoteNameServiceServer;

/**
 * Main program for NameServiceClient. This program makes a connection to the
 * listener created by MainNameServiceListener.
 */
public class MainNameServiceClient implements NameServiceHelper.NameServiceClientFactory
{
	/**
	 * Main program for NameServiceClient.
	 * 
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		// TODO Change to correct URI
		String uri = "tcp://localhost:4001";
		
		RemoteNameServiceServer server = NameServiceHelper.newServer( uri, null,
			new MainNameServiceClient() );

		// Connect to the service
		server._startAndWaitUp( 4000 );

		// TODO Insert Your Code Here

		// Disconnect from the service
		server._stopAndWaitDown( 4000 );
	}

	public NameServiceClient newNameServiceClient( RemoteNameServiceServer server )
		throws Exception
	{
		return new ImplNameServiceClient( server );
	}
}
