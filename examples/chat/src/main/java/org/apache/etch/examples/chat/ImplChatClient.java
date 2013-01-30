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

import java.util.HashSet;
import java.util.Set;

import org.apache.etch.util.core.io.Session;



/**
 * Your custom implementation of BaseChatClient. Add methods here to provide
 * implementations of messages from the server.
 */
public class ImplChatClient extends BaseChatClient
{
	/**
	 * Constructs the ImplChatClient.
	 *
	 * @param server a connection to the server session. Use this to send a
	 * message to the server.
	 * @param whoami 
	 * @param snub 
	 */
	public ImplChatClient( RemoteChatServer server, String whoami, boolean snub )
	{
		this.server = server;
		this.whoami = whoami;
		this.snub = snub;
	}
	
	private final RemoteChatServer server;
	
	private final String whoami;
	
	private final boolean snub;

	@Override
	public Boolean isLoggedIn()
	{
		// server is always logged in to the client.
		return true;
	}

	@Override
	public void statusChange( String name, Boolean online ) 
	{
		if (online)
		{
			System.out.printf( "%s: %s online (src=statusChange)\n", whoami, name );
			whoIsOnline.add( name );
		}
		else
		{
			System.out.printf( "%s: %s offline\n", whoami, name );
			whoIsOnline.remove( name );
		}
	}

	@Override
	public void whoIsOnline( String[] names )
    {
		for (String name: names)
		{
			System.out.printf( "%s: %s online (src=whoIsOnline)\n", whoami, name );
			whoIsOnline.add(  name );
		}
    }
	
	private Set<String> whoIsOnline = new HashSet<String>();

	@Override
	public void send( String who, String msg ) throws Failure
    {
		System.out.printf( "%s: msg from %s: %s\n", whoami, who, msg );
		if (snub)
		{
			server.send( who, String.format( "%s, leave me alone!", who ) );
			System.out.println( "snub done" );
		}
    }

	@Override
	public void _sessionNotify( Object event ) throws Exception
	{
		if (event == Session.DOWN)
			System.out.println( "(server disconnected)" );
	}
}