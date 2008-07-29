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

import java.net.InetSocketAddress;

import etch.bindings.java.support.ServerFactory;
import etch.util.URL;
import etch.util.core.io.TcpListener;
import etch.util.core.io.Transport;

/**
 * Main program for InheritanceServer. This program makes a listener to accept
 * connections from MainInheritanceClient.
 */
public class MainInheritanceListener implements InheritanceHelper.InheritanceServerFactory
{
	/**
	 * Main program for InheritanceServer.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String uri = "tcp://0.0.0.0:4001";
		
		Transport<ServerFactory> listener = InheritanceHelper.newListener( uri, null,
			new MainInheritanceListener() );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		InetSocketAddress localAddr = (InetSocketAddress) listener.transportQuery( TcpListener.LOCAL_ADDRESS );
		URL u = new URL( "tcp:" );
		u.setHost( localAddr.getHostName() );
		u.setPort( localAddr.getPort() );
		localUri = u.toString();
	}
	
	/**
	 * The local uri of the listener.
	 */
	public static String localUri;

	public InheritanceServer newInheritanceServer( RemoteInheritanceClient client )
	{
		return new ImplInheritanceServer( client );
	}
}
