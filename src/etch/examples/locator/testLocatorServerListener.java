package etch.examples.locator;

import java.util.ArrayList;
import java.util.List;

import metreos.core.io.Connection;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.util.Log;
import etch.examples.locator.Locator.Registration;

/**
 * @author bchampak
 * 
 */
public class testLocatorServerListener
{
	/**
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "testLocatorServerListener", "what", "starting" );
		
		int port;

		if (args.length > 0)
		{
			if (args.length != 1 || args[0].equals( "-help" ))
			{
				System.out.println( "usage: testLocatorServerListener [port]" );
				System.exit( 1 );
			}
			port = Integer.parseInt( args[0] );
		}
		else
		{
			port = 4001;
		}

		final List<Registration> contexts = new ArrayList<Registration>();

//		contexts.add( new Context( "Locator", "badri", "localhost", port ) ); // 6001
//
//		contexts.add( new Context( "Authenticator", "jan", "localhost", 6002 ) );
//		contexts.add( new Context( "Authenticator", "scott", "localhost", 6003 ) );
//
//		contexts.add( new Context( "Configuration", "jan", "localhost", 6004 ) );
//		contexts.add( new Context( "Configuration", "scott", "localhost", 6005 ) );
//
//		contexts.add( new Context( "Log", "louis", "localhost", 6006 ) );

		ListenerHandler lh = LocatorHelper.newListenerHandler( contexts );
		Connection<ListenerHandler> l = new Listener( lh, 5, null, port, 0 );
		
		l.start();
		l.waitUp( 4000 );
		
		Log.report( "testLocatorServerListener", "what", "listening", "address", l.localAddress() );
	}
}