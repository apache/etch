package etch.examples.chat;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Collections;
import java.util.Date;
import java.util.HashSet;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.StringTokenizer;

import metreos.util.Log;
import metreos.util.URL;
import etch.bindings.java.support._Etch_AuthException;
import etch.bindings.java.transport.Session;

/**
 * Simple command line chat client.
 */
public class Chatter implements ChatClient, Session
{
	/**
	 * @param args
	 * @throws Exception 
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "Chatter" );
		
		URL uri;
		if (args.length == 1)
		{
			uri = new URL( args[0] );
		}
		else if (args.length == 0)
		{
			uri = new URL( "tcp://localhost:4001" );
		}
		else
		{
			System.out.println( "usage: Chatter <uri>" );
			System.exit( 1 );
			return;
		}
		
		System.out.println( "Connecting to chat server at "+uri );

		Chatter client = new Chatter();
		RemoteChatServer server = ChatHelper.newChatServer( uri, null, client );
		server._startAndWaitUp( 4000 );
		
		System.out.println( "(connected, enter command or 'help' to list commands)" );
		
		BufferedReader rdr = new BufferedReader( new InputStreamReader( System.in ) );
		while (true)
		{
			System.out.print( "cmd: " );
			System.out.flush();
			
			String s = rdr.readLine();
			if (s.length() == 0)
				continue;
			
			StringTokenizer st = new StringTokenizer( s, " \t\r\n" );
			if (!st.hasMoreTokens())
				continue;
			
			String cmd = st.nextToken();
			
			if (cmd.equals( "help" ))
			{
				do_help();
				continue;
			}
			
			if (cmd.equals( "login" ))
			{
				do_login( server, st );
				continue;
			}
			
			if (cmd.equals( "logout" ))
			{
				do_logout( server );
				continue;
			}
			
			if (cmd.equals( "quit" ))
				break;
			
			if (cmd.equals( "send" ))
			{
				do_send( server, cmd, st );
				continue;
			}
			
			if (cmd.equals( "who" ))
			{
				do_who( client );
				continue;
			}

			System.out.println( "unknown command "+cmd );
			do_help();
		}
		
		server._stop();
	}

	private static void do_help()
    {
	    System.out.println( "commands: " );
	    System.out.println( "  help" );
	    System.out.println( "  login <user> <pwd>" );
	    System.out.println( "  logout" );
	    System.out.println( "  quit" );
 	    System.out.println( "  send <user> <msg>" );
 	    System.out.println( "  who" );
	}

	private static void do_login( ChatServer server, StringTokenizer st )
    {
		try
		{
			String name = st.nextToken();
			String pwd = st.nextToken();
			server.login( name, pwd );
			System.out.println( "(login successful)" );
		}
		catch ( NoSuchElementException e )
		{
			System.out.println( "usage: login <user> <pwd>" );
		}
		catch ( Failure e )
		{
			System.out.println( "failed: "+e.getMessage() );
			e.printStackTrace();
		}
		catch ( Exception e )
		{
			System.out.println( "failed: "+e );
			e.printStackTrace();
		}
    }

	private static void do_logout( ChatServer server )
	{
		server.logout();
		System.out.println( "(logout successful)" );
	}

	private static void do_send( ChatServer server, String cmd, StringTokenizer st )
    {
		try
		{
			String who = st.nextToken();
			String msg = st.nextToken( "\r\n" );
			server.send( who, msg );
		}
		catch ( NoSuchElementException e )
		{
			System.out.println( "usage: send <user> <msg>" );
		}
		catch ( _Etch_AuthException e )
		{
			System.out.println( "you must login first" );
		}
		catch ( Failure e )
		{
			System.out.println( "failed: "+e.getMessage() );
		}
		catch ( Exception e )
		{
			System.out.println( "failed: "+e );
		}
    }
	
	private static void do_who( Chatter client )
    {
	    for (String name: client.getWhoIsOnline())
	    	System.out.println( "  "+name );
    }
	
	/**
	 * @return a list of who is online right now.
	 */
	public String[] getWhoIsOnline()
	{
		return whoIsOnline.toArray( new String[0] );
	}
	
	private final Set<String> whoIsOnline = Collections.synchronizedSet( new HashSet<String>() );

    ////////////////
    // ChatClient //
    ////////////////

	public void whoIsOnline( String[] names )
    {
	    for (String name: names)
	    {
	    	whoIsOnline.add( name );
	    	System.out.println( name+" is online" );
	    }
    }

	public void statusChange( String name, Boolean online )
    {
		if (online)
		{
			whoIsOnline.add( name );
			System.out.println( name+" is now online" );
		}
		else
		{
			whoIsOnline.remove( name );
			System.out.println( name+" is now offline" );
		}
    }

	//////////
	// Chat //
	//////////

	public Boolean isLoggedIn()
    {
		// server is always logged in to the client.
	    return true;
    }

	public void send( String who, String msg ) throws Failure
    {
		synchronized( df )
		{
			System.out.printf( "[ %s@%s ] %s\n", who, df.format( new Date() ), msg );
		}
    }
	
	private DateFormat df = new SimpleDateFormat( "HH:mm:ss" );

	////////////////
	// OOBHandler //
	////////////////

	public Object sessionQuery( Object query )
	{
		throw new UnsupportedOperationException( "unknown query: "+query );
	}

	public void sessionControl( Object control, Object value )
	{
		throw new UnsupportedOperationException( "unknown control: "+control );
	}

	public void sessionNotify( Object event )
	{
		if (event.equals( Session.UP ) )
		{
			System.out.println( "(server opened connection)" );
			return;
		}
		
		if (event.equals( Session.DOWN ) )
		{
			System.out.println( "(server closed connection)" );
			return;
		}
		
		if (event instanceof Throwable)
		{
			((Throwable) event).printStackTrace();
			return;
		}
	}
}
