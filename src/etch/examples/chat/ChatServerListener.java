package etch.examples.chat;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import metreos.util.Log;
import metreos.util.URL;
import etch.bindings.java.transport.Transport;

/**
 * Starts a Chat listener.
 */
public class ChatServerListener implements ChatServerFactory
{
	/**
	 * Program entry point.
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "ChatServerListener" );
		
		URL uri;
		if (args.length == 1)
		{
			uri = new URL( args[0] );
		}
		else if (args.length == 0)
		{
			uri = new URL( "tcp://0.0.0.0:4001" );
		}
		else
		{
			System.out.println( "usage: ChatServerListener <uri>" );
			System.exit( 1 );
			return;
		}
		
		ChatServerListener implFactory = new ChatServerListener();
		Transport src = ChatHelper.newChatListener( uri, null, implFactory );
		src.transportControl( Transport.START, null );
	}

	/**
	 * Constructs the ChatServer factory.
	 */
	public ChatServerListener()
	{
		// nothing to do.
	}

	private final Map<String, ChatServerImpl> whoIsOnline =
		Collections.synchronizedMap( new HashMap<String, ChatServerImpl>() );
	
	public ChatServer newChatServer( ChatClient client ) throws Exception
	{
		return new ChatServerImpl( client, whoIsOnline );
	}
}
