
package etch.examples.log;


/**
 * @author bchampak
 */
public class testRemoteLogServer
{

	/**
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String host;
		int port;
		String instance;

		if (args.length > 0)
		{
			if (args.length != 3 || args[0].equals( "-help" ))
			{
				System.out.println( "usage: testRemoteLogServer [host port instance]" );
				System.exit( 1 );
			}
			host = args[0];
			port = Integer.parseInt( args[1] );
			instance = args[2];
		}
		else
		{
			host = "localhost";
			port = 4006;
			instance = LogHelper.LOCAL;
		}
		
		LogServer server = LogHelper.getLogInstance( host, port, instance );

//		server.login( "Mary", "cisco456" );
//		System.out.println( "Is Mary Logged in: " + server.isLoggedIn() );
//		server.logout();
//		System.out.println( "Is Mary Logged in: " + server.isLoggedIn() );

//		server.login( "Fred", "cisco123" );
//		System.out.println( "Is Fred Logged in: " + server.isLoggedIn() );
//		server.logout();
//		System.out.println( "Is Fred Logged in: " + server.isLoggedIn() );

		server.login( "Fred", "cisco123" );

		server.setLevel( Log.WARNING );
		System.out.println( "Level set to warning for Fred: " + server.getLevel() );
		server.log( Log.DEBUG, "debug msg 1 (better not see it)" );
		server.log( Log.VERBOSE, "verbose msg 1 (better not see it)" );
		server.log( Log.INFO, "info msg 1 (better not see it)" );
		server.log( Log.WARNING, "warning msg 1" );
		server.log( Log.ERROR, "error msg 1" );

		server.setLevel( Log.VERBOSE );
		System.out.println( "Level set to verbose for Fred: " + server.getLevel() );
		server.log( Log.DEBUG, "debug msg 2 (better not see it)" );
		server.log( Log.VERBOSE, "verbose msg 2" );
		server.log( Log.INFO, "info msg 2" );
		server.log( Log.WARNING, "warning msg 2" );
		server.log( Log.ERROR, "error msg 2" );
		
		server.logout();
		System.out.println( "Is Fred Logged in: " + server.isLoggedIn() );

//		try
//		{
//			server.login( "Joe", "cisco789" );
//			System.out.println( "Is Joe Logged in: " + server.isLoggedIn() );
//			server.logout();
//			System.out.println( "Is Joe Logged in: " + server.isLoggedIn() );
//		}
//		catch ( Exception e )
//		{
//			System.out.println( "You are not Allowed to login " + e );
//		}
		
		((RemoteLogServer) server)._stop();
	}
}
