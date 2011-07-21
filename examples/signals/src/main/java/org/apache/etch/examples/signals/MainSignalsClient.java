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

/**
 * Main program for SignalsClient.
 */
public class MainSignalsClient implements SignalsHelper.SignalsClientFactory
{
	private static String clientName;

	protected static void setClientName(String clientName) {
		MainSignalsClient.clientName = clientName;
	}

	public static String getClientName() {
		return clientName;
	}

	/**
	 * Main program for SignalsClient.
	 *
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String uri = "udp://localhost:4001";

		RemoteSignalsServer server = SignalsHelper.newServer( uri, null, new MainSignalsClient() );

		// Connect to the service
		server._startAndWaitUp( 4000 );

		try {
			Scanner console = new Scanner(System.in);
			setClientName(server.get_free_client_name());
			System.out.println("Got client name: " + getClientName());

			System.out.println(getClientName() + ": Subscribing to example signal ...");
			server._SubscribeExampleSignal();

			System.out.println(getClientName() + ": Press enter to unsubscribe");
			console.nextLine();
			server._UnsubscribeExampleSignal();

			// Disconnect from the service
			System.out.println(getClientName() + ": Press enter to disconnect");
			console.nextLine();
			server._stopAndWaitDown( 4000 );
		} catch (Exception ex) {
			System.out.println("Error: " + ex.getMessage());
		}
	}

	public SignalsClient newSignalsClient( RemoteSignalsServer server ) throws Exception
	{
		return new ImplSignalsClient( server );
	}
}
