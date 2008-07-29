package etch.examples.authentication;

import metreos.core.io.Connection;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.util.Log;
import etch.examples.configuration.Configuration;

/**
 * @author bchampak
 */
public class testAuthenticatorServerListener
{
	/**
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "testAuthenticatorServerListener", "what", "starting" );

		int port;
		
		if (args.length > 0)
		{
			if (args.length != 1 || args[0].equals( "-help" ))
			{
				System.out.println( "usage: testAuthenticatorServerListener [port]" );
				System.exit( 1 );
			}
			port = Integer.parseInt( args[0] );
		}
		else
		{
			port = 4002;
		}

		final AuthDb auth = new AuthDb();

		auth.addUser( "Fred", "cisco123" );
		auth.grantRight( "Fred", "Log.write" );

		auth.addUser( "Mary", "cisco456" );
		auth.grantRight( "Mary", Authenticator.ADMIN_RIGHT );
		auth.grantRight( "Mary", Authenticator.CHECK_RIGHT );

		auth.addUser( "Joe", "cisco789" );
		auth.grantRight( "Joe", Authenticator.CHECK_RIGHT );
		auth.grantRight( "Joe", Configuration.READ_RIGHT );

		ListenerHandler lh = AuthenticatorHelper.newListenerHandler( auth );
		Connection<ListenerHandler> l = new Listener( lh, 5, null, port, 0 );
		
		l.start();
		l.waitUp( 4000 );
		
		Log.report( "testAuthenticatorServerListener", "what", "listening", "address", l.localAddress() );
	}
}
