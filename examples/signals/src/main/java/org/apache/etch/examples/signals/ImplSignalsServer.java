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

import java.util.Timer;
import java.util.TimerTask;

import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Session;

/**
 * Custom implementation of BaseSignalsServer.
 */
public class ImplSignalsServer extends BaseSignalsServer
{
	private static int clientCount = 0;
	private Timer signalTimer = null;
	private int signalCount = 0;

	/**
	 * Constructs the ImplSignalsServer.
	 *
	 * @param client a connection to the client session.
	 */
	public ImplSignalsServer( RemoteSignalsClient client )
	{
		super( client );
	}

	@Override
	public String get_free_client_name() {
		String clientName = "client" + clientCount;

		clientCount++;

		System.out.println( "Allocated client name: " + clientName );
		return clientName;
	}

	@Override
	public void _sessionNotify(Object event) throws Exception {
		if (Session.DOWN.equals(event)) {
			System.out.println( "Session DOWN, stopping timer" );
			disposeTimer();
		}
	}

	@Override
	public void _SubscribeExampleSignal(Who sender) throws Exception {
		super._SubscribeExampleSignal( sender );
		System.out.println( "Subscribed " + sender );

		if ( signalTimer == null )
		{
			System.out.println( "Starting timer" );

			signalTimer = new Timer();
			signalTimer.schedule(new NotifyClientsTask(), 1000, 1000);
		}
	}

	@Override
	public void _UnsubscribeExampleSignal(Who sender) throws Exception {
		super._UnsubscribeExampleSignal( sender );
		System.out.println( "Unsubscribed " + sender );

		if ( client._getExampleSubscribers().length < 1 )
		{
			System.out.println( "No subscribers, stopping timer" );
			disposeTimer();
		}
	}

	private void disposeTimer() {
		if (signalTimer != null) {
			signalTimer.cancel();
			signalTimer = null;
		}
	}

	class NotifyClientsTask extends TimerTask {
		@Override
		public void run() {
			signalCount++;
			System.out.println("Sending event to " + client._getExampleSubscribers().length + " subscribers (" + signalCount + ")");
			client.example_signal(signalCount);
		}
	}

}
