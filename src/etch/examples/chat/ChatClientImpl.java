package etch.examples.chat;

import java.util.HashSet;
import java.util.Set;

import etch.bindings.java.transport.Session;

/**
 * Test jig vs. ChatServerImpl.
 */
public class ChatClientImpl implements ChatClient, Session
{
	/**
	 * Constructs the ChatClientImpl.
	 * @param whoami
	 * @param snub
	 */
	public ChatClientImpl( String whoami, boolean snub )
	{
		this.whoami = whoami;
		this.snub = snub;
	}
	
	private final String whoami;
	
	private final boolean snub;

	public Boolean isLoggedIn()
	{
		// server is always logged in to the client.
		return true;
	}

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

	public void whoIsOnline( String[] names )
    {
		for (String name: names)
		{
			System.out.printf( "%s: %s online (src=whoIsOnline)\n", whoami, name );
			whoIsOnline.add(  name );
		}
    }
	
	private Set<String> whoIsOnline = new HashSet<String>();

	public void send( String who, String msg ) throws Failure
    {
		System.out.printf( "%s: msg from %s: %s\n", whoami, who, msg );
		if (snub)
		{
			server.send( who, String.format( "%s, leave me alone!", who ) );
			System.out.println( "snub done" );
		}
    }

	/**
	 * Sets the server so that we can send messages to it.
	 * @param server
	 */
	public void setServer( ChatServer server)
	{
		this.server = server;
	}
	
	private ChatServer server;

	public Object sessionQuery( Object query ) throws Exception
	{
		throw new UnsupportedOperationException( "unknown query "+query );
	}

	public void sessionControl( Object control, Object value ) throws Exception
	{
		throw new UnsupportedOperationException( "unknown control "+control );
	}

	public void sessionNotify( Object event ) throws Exception
	{
		if (event == Session.DOWN)
			System.out.println( "(server disconnected)" );
	}
}
