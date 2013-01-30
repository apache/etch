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

import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import org.apache.etch.util.Log;
import org.apache.etch.util.core.io.Session;



/**
 * Your custom implementation of BaseChatServer. Add methods here to provide
 * implementations of messages from the client.
 */
public class ImplChatServer extends BaseChatServer
{
	/**
	 * Constructs the ImplChatServer.
	 *
	 * @param client a connection to the client session. Use this to send a
	 * message to the client.
	 * @param whoIsOnline database of online connections.
	 */
	public ImplChatServer( RemoteChatClient client,
		Map<String, ImplChatServer> whoIsOnline )
	{
		this.client = client;
		this.whoIsOnline = whoIsOnline;
	}
	
	private final RemoteChatClient client;

	private final Map<String, ImplChatServer> whoIsOnline;

	////////////////////
	// Authentication //
	////////////////////
	
//	private final static String INVALID_NAME_PWD_MSG = "User name/password is not valid";

	@Override
	public synchronized void login( String name, String pwd ) throws Failure
	{
		if (isLoggedIn())
			throw new Failure( "Already logged in" );

		if (name == null)
			throw new Failure("Username is null");
		
		if (name.length() == 0)
			throw new Failure("Username is empty");
		
		if (name.startsWith("bad"))
			throw new Failure("Username is not valid");

		if (pwd == null)
			throw new Failure("Password is null");
		
		if (pwd.startsWith("bad"))
			throw new Failure("Password is not valid");
		
		// TODO check user authentication.

		// add this user to the who is online map.
		
		ImplChatServer other = whoIsOnline.put( name, this );
		if (other != null)
		{
			try
			{
				other.deliver( "SYSTEM", "You have been logged out because you logged in somewhere else." );
			}
			catch ( Exception e )
			{
				// ignore failure to send this message.
			}
			other.logout();
			other = null;
		}
		
		// mark as logged in.

		user = name;
		
		// say we're online
		
		sayStatusChanged( user, true );

		// tell the client who's online
		
		Set<String> who;
		synchronized (whoIsOnline)
		{
			who = new HashSet<String>( whoIsOnline.keySet() );
		}
		who.remove( name );
		client.whoIsOnline( who.toArray( new String[who.size()] ) );
		
		Log.report( "login", "who", name );
	}

	@Override
	public synchronized void logout()
	{
		if (user != null)
		{
			boolean tookUsOfflne = false;
			
			synchronized (whoIsOnline)
			{
				if (whoIsOnline.get( user ) == this)
				{
					// remove us from the online map.
					
					whoIsOnline.remove( user );
					tookUsOfflne = true;
				}
			}
			
			if (tookUsOfflne)
			{
				// say we're offline
				
				sayStatusChanged( user, false );

				Log.report( "logout", "who", user );
			}
			
			user = null;
		}
	}

	@Override
	public Boolean isLoggedIn()
	{
		return user != null;
	}

	private void sayStatusChanged( String u, boolean isOnline )
    {
		Collection<ImplChatServer> who = whoIsOnline.values();
	    for (ImplChatServer other: who.toArray( new ImplChatServer[who.size()] ))
	    	if (other != this)
	    		other.client.statusChange( u, isOnline );
    }

	private String user;

	//////////
	// Send //
	//////////

	@Override
	public void send( String who, String msg ) throws Failure
	{
		String me = user;
		if (me == null)
			return;
		
		ImplChatServer other = whoIsOnline.get( who );
		if (other == null)
			throw new Failure( "user is not online: "+who );
		
		other.deliver( me, msg );
	}

	private void deliver( String from, String msg ) throws Failure
	{
		// TODO check from not in user's block list...
		
		try
		{
			Log.report( "sending", "from", from, "to", user, "msg", msg );
			client.send( from, msg );
			Log.report( "sent", "from", from, "to", user, "msg", msg );
		}
		catch ( Failure e )
		{
			e.printStackTrace();
			throw e;
		}
		catch ( RuntimeException e )
		{
			e.printStackTrace();
			throw e;
		}
	}

	////////////////////////////////////////
	// Session query, control, and notify //
	////////////////////////////////////////

	@Override
	public void _sessionNotify( Object event ) throws Exception
	{
		if (event == Session.UP)
			return;
		
		if (event == Session.DOWN)
		{
			logout();
			return;
		}
		
		if (event instanceof Throwable)
			((Throwable) event).printStackTrace();
		else
			Log.report( "event", "conn", this, "what", event );
	}
}