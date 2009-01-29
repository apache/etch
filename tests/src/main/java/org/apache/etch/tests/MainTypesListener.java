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

package org.apache.etch.tests;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.tests.RemoteTypesClient;
import org.apache.etch.tests.TypesHelper;
import org.apache.etch.tests.TypesServer;
import org.apache.etch.util.core.io.Transport;



/**
 * Main program for TypesServer. This program makes a listener to accept
 * connections from MainTypesClient.
 */
public class MainTypesListener implements TypesHelper.TypesServerFactory
{
	/**
	 * Main program for TypesServer.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String uri = "tcp://0.0.0.0:4001";
		
		Transport<ServerFactory> listener = TypesHelper.newListener( uri, null,
			new MainTypesListener() );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}

	public TypesServer newTypesServer( RemoteTypesClient client )
	{
		return new ImplTypesServer( client );
	}
}
