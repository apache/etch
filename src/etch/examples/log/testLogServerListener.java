
package etch.examples.log;

import metreos.core.io.Connection;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.util.Log;
import etch.examples.authentication.AuthDb;
import etch.examples.authentication.AuthenticatorHelper;
import etch.examples.authentication.AuthenticatorServer;
import etch.examples.authentication.Authenticator.Failure;
import etch.examples.configuration.Configuration;

/**
 * @author bchampak
 */
public class testLogServerListener
{
	/**
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "testLogServerListener", "what", "starting" );
		
		int port;
		String authHost;
		int authPort;
		String authInstance;
		String authName;
		String authPwd;
		
		if (args.length > 0)
		{
			if (args.length != 6 || args[0].equals( "-help" ))
			{
				System.out.println( "usage: testLogServerListener [port authHost authPort authInstance authName authPwd]" );
				System.exit( 1 );
			}
			port = Integer.parseInt( args[0] );
			authHost = args[1];
			authPort = Integer.parseInt( args[2] );
			authInstance = args[3];
			authName = args[4];
			authPwd = args[5];
		}
		else
		{
			port = 4006;
			authHost = "localhost";
			authPort = 4002;
			authInstance = AuthenticatorHelper.LOCAL;
			authName = "Mary";
			authPwd = "cisco456";
		}
		
		final AuthenticatorServer auth;

		if (true)
		{
			auth = AuthenticatorHelper.getAuthenticatorInstance( authHost, authPort, authInstance );
			auth.login( authName, authPwd );
		}
		else
		{
			auth = setupAuthDb();
		}
		
		final LogSink lsink = new LogSink()
		{
			public void write( int level, String msg )
			{
				Log.report( "MESSAGE", "level", level, "msg", msg );
			}
		};

		ListenerHandler lh = LogHelper.newListenerHandler( auth, lsink );
		Connection<ListenerHandler> l = new Listener( lh, 5, null, port, 0 );
		
		l.start();
		l.waitUp( 4000 );
		
		Log.report( "testLogServerListener", "what", "listening", "address", l.localAddress() );
	}

	private static AuthDb setupAuthDb() throws Failure
	{
		AuthDb auth = new AuthDb();
		
		auth.addUser( "Fred", "cisco123" );
		auth.grantRight( "Fred", "Log.write" );
		
		auth.addUser( "Mary", "cisco456" );
		auth.grantRight( "Mary", "Auth.admin" );
		auth.grantRight( "Mary", "Auth.check" );

		auth.addUser( "Joe", "cisco789" );
		auth.grantRight( "Joe", Configuration.READ_RIGHT );
		
		return auth;
	}
}
