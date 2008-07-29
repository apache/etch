package etch.examples.chat;

import java.util.Collection;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;

import metreos.util.Log;
import etch.bindings.java.transport.Session;

/**
 * Implementation of ChatServer.
 */
public class ChatServerImpl implements ChatServer, Session
{
	/**
	 * Constructs the ChatServerImpl.
	 * @param client the client to use for callbacks.
	 * @param whoIsOnline the database of who is online.
	 */
	public ChatServerImpl( ChatClient client, Map<String, ChatServerImpl> whoIsOnline )
	{
		this.client = client;
		this.whoIsOnline = whoIsOnline;
	}

	private final ChatClient client;

	private final Map<String, ChatServerImpl> whoIsOnline;

	////////////////////
	// Authentication //
	////////////////////
	
	private final static String INVALID_NAME_PWD_MSG = "User name/password is not valid";

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
		
		ChatServerImpl other = whoIsOnline.put( name, this );
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

	public Boolean isLoggedIn()
	{
		return user != null;
	}

	private void sayStatusChanged( String u, boolean isOnline )
    {
		Collection<ChatServerImpl> who = whoIsOnline.values();
	    for (ChatServerImpl other: who.toArray( new ChatServerImpl[who.size()] ))
	    	if (other != this)
	    		other.client.statusChange( u, isOnline );
    }

	private String user;

	//////////
	// Send //
	//////////

	public void send( String who, String msg ) throws Failure
	{
		String me = user;
		if (me == null)
			return;
		
		ChatServerImpl other = whoIsOnline.get( who );
		if (other == null)
			throw new Failure( "user is not online: "+who );
		
		other.deliver( me, msg );
	}

	private void deliver( String from, String msg ) throws Failure
	{
		// TODO check from not in user's block list...
		
		Log.report( "sending", "from", from, "to", user, "msg", msg );
		client.send( from, msg );
		Log.report( "sent", "from", from, "to", user, "msg", msg );
	}

	////////////////////////////////////////
	// Session query, control, and notify //
	////////////////////////////////////////

	public Object sessionQuery( Object query ) throws Exception
	{
		throw new UnsupportedOperationException( "unknown query"+query );
	}

	public void sessionControl( Object control, Object value ) throws Exception
	{
		throw new UnsupportedOperationException( "unknown control"+control );
	}

	public void sessionNotify( Object event ) throws Exception
	{
		if (event == Session.UP)
			logout();
	}
}
