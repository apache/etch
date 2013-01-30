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

package org.apache.etch.examples.chat;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.util.Log;
import org.apache.etch.util.core.io.Transport;



/**
 * Main program for ChatServer. This program makes a listener to accept
 * connections from MainChatClient.
 */
public class MainChatListener implements ChatHelper.ChatServerFactory
{
	/**
	 * Main program for ChatServer.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "MainChatListener" );
		
		String uri = "tcp://0.0.0.0:4005";
		if (args.length > 0)
		{
			uri = args[0];
		}
		else if (args.length != 0)
		{
			System.out.println( "usage: MainChatListener [uri]" );
			System.exit( 1 );
			return;
		}
		
		MainChatListener implFactory = new MainChatListener();
		
		ServerFactory listener = ChatHelper.newListener( uri, null,
			implFactory );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}

	private final Map<String, ImplChatServer> whoIsOnline =
		Collections.synchronizedMap( new HashMap<String, ImplChatServer>() );

	public ChatServer newChatServer( RemoteChatClient client )
	{
		return new ImplChatServer( client, whoIsOnline );
	}
}
