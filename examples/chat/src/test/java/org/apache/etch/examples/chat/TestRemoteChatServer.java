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

// import static org.junit.Assert.*; 

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.examples.chat.Chat.Failure;
import org.apache.etch.util.Log;
import org.apache.etch.util.core.io.Transport;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;


/**
 * Tests RemoteChatServer.
 */
public class TestRemoteChatServer
{
	final String user = "user1";
	final String nullUser = null;
	final String badUser = "baduser";
	final String badPass = "badPass"; 
	RemoteChatServer cs;

	/**
	 * Start remote chat listener
	 * @throws Exception
	 */
	@BeforeClass
	public static void startRemoteChatListener() throws Exception 
	{
		Log.addSink(null);
		Log.report("StartRemoteChatListener");
		
		String uri = "tcp://localhost:4005";
		MainChatListener implFactory = new MainChatListener(); 
		listener = ChatHelper.newListener( uri, null, implFactory );
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		Log.report( "StartRemoteChatListener",
			"status", "up",
			"listener", listener );
	}
	
	/**
	 * @throws Exception
	 */
	@AfterClass
	public static void stopRemoteChatListener() throws Exception
	{
		if (listener != null)
		{
			listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
			listener = null;
		}
	}
	
	private static ServerFactory listener;
	
	/**
	 * testLogin1: Login user
	 * @throws Exception
	 */
	@Test
	public void testLogin1() throws Exception
	{		
		Log.report("TestRemoteChatServer.login: testLogin1 - Login user");
		cs = setupAndConnectUser(user);
		cs.login(user,user);
	}
	
	/**
	 * testLogin2: User is already logged in
	 * @throws Exception
	 */
	@Test(expected = Failure.class)
	public void testLogin2() throws Exception
	{
		Log.report("TestRemoteChatServer.login: testLogin2 - Login user. User is already logged in");
		cs = setupAndConnectUser(user);
		cs.login(user,user);
		cs.login(user,user);
	}
	
	/**
	 * testLogin3: Login - user is null
	 * @throws Exception
	 */
	@Test(expected = Failure.class)
	public void testLogin3() throws Exception
	{		
		Log.report("TestRemoteChatServer.login: testLogin3 - Login user. User is null");
		cs = setupAndConnectUser(nullUser);
		cs.login(nullUser,nullUser);
	}
	
	/**
	 * testLogin4: Login - bad password
	 * @throws Exception
	 */
	@Test(expected = Failure.class)
	public void testLogin4() throws Exception
	{		
		Log.report("TestRemoteChatServer.login: testLogin4 - Login user. Bad password");
		cs = setupAndConnectUser(user);
		cs.login(user,badPass);
	}
	
	/**
	 * testLogin5: Login - bad username
	 * @throws Exception
	 */
	@Test(expected = Failure.class)
	public void testLogin5() throws Exception
	{		
		Log.report("TestRemoteChatServer.login: testLogin5 - Login user. Bad username");
		cs = setupAndConnectUser(user);
		cs.login(user,badUser);
	}
	
	/**
	 * Logout out user and close TCP conection
	 * @throws Exception 
	 */
	@After
	public void cleanup() throws Exception {
		if (cs != null)
		{
			cs.logout();
			cs._stop();
		}
	}
	
	/**
	 * @param user
	 * @return chat server
	 * @throws Exception
	 */
	public RemoteChatServer setupAndConnectUser (final String user) throws Exception
	{
		String uri = "tcp://localhost:4005";
		
//		ChatClientImpl client = new ChatClientImpl(user, false);
		RemoteChatServer server = ChatHelper.newServer( uri, null,
			new ChatHelper.ChatClientFactory()
			{
				public ChatClient newChatClient( RemoteChatServer server )
					throws Exception
				{
					return new ImplChatClient( server, user, false );
				}
			} );
//		client._setServer(server);
	
		server._startAndWaitUp( 4000 );
		
		return server;
	}
}
