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

import etch.bindings.java.support.ServerFactory;
import etch.util.Log;
import etch.util.core.io.Transport;

/**
 * Main program for AsyncServer. This program makes a listener to accept
 * connections from MainAsyncClient.
 */
public class MainAsyncListener implements AsyncHelper.AsyncServerFactory
{
	/**
	 * Main program for AsyncServer.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "MainAsyncListener" );
		
		String uri = "tcp://0.0.0.0:4003";

		Transport<ServerFactory> listener = AsyncHelper.newListener( uri, null,
			new MainAsyncListener() );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}

	public AsyncServer newAsyncServer( RemoteAsyncClient client )
	{
		return new ImplAsyncServer( client );
	}
}
