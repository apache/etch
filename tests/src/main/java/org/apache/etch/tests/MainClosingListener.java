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

import java.net.InetSocketAddress;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.io.TcpListener;
import org.apache.etch.util.core.io.Transport;



/**
 * Main implementation for ClosingListener.
 */
public class MainClosingListener implements ClosingHelper.ClosingServerFactory
{
	/**
 	 * Main for ClosingListener.
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args)
		throws Exception
	{
		String uri = "tcp://0.0.0.0:4006";
		
		ServerFactory listener = ClosingHelper.newListener( uri, null, new MainClosingListener() );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		InetSocketAddress localAddr = (InetSocketAddress) listener.transportQuery( TcpListener.LOCAL_ADDRESS );
		URL u = new URL( "tcp:" );
		u.setHost( localAddr.getHostName() );
		u.setPort( localAddr.getPort() );
		localUri = u.toString();
	}

	/**
	 * @return the uri of our listener.
	 */
	public static String getLocalUri()
	{
		return localUri;
	}
	
	private static String localUri;

	/**
	 * Constructs the MainClosingListener.
	 */
	public MainClosingListener()
	{
		// Nothing to do.
	}

	/**
	 * Return a new instance of ClosingServer.
	 * @param client
	 * @return the constructed server.
	 */
	public ClosingServer newClosingServer( RemoteClosingClient client )
	{
		return new ImplClosingServer( client );
	}
}
