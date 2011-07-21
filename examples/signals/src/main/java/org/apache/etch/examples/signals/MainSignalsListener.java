/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

package org.apache.etch.examples.signals;

import java.util.Scanner;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.util.core.io.Transport;

/**
 * Main program for SignalsServer.
 */
public class MainSignalsListener implements SignalsHelper.SignalsServerFactory
{
	/**
	 * Main program for SignalsServer.
	 *
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String uri = "udp://0.0.0.0:4001?UdpListener.singleSession=true";

		ServerFactory listener = SignalsHelper.newListener( uri, null, new MainSignalsListener() );

		// Start the Listener
		System.out.println("Starting Server: " + uri);
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );

		System.out.println( "Press enter to stop server" );
		Scanner console = new Scanner(System.in);
		console.nextLine();
		listener.transportControl( Transport.STOP, 0 );
	}

	public SignalsServer newSignalsServer( RemoteSignalsClient client )
	{
		return new ImplSignalsServer( client );
	}
}
