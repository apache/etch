
package etch.examples.locator;

import metreos.util.URL;
import etch.examples.locator.Locator.Registration;

/**
 * @author bchampak
 * 
 */
public class testRemoteLocatorServer
{
	/**
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String host;
		int port;
		
		if (args.length > 0)
		{
			if (args.length != 2 || args[0].equals( "-help" ))
			{
				System.out.println( "usage: testRemoteLocatorServer [host port]" );
				System.exit( 1 );
			}
			host = args[0];
			port = Integer.parseInt( args[1] );
		}
		else
		{
			host = "localhost";
			port = 4001;
		}
		
		LocatorServer ls = LocatorHelper.getLocator( host, port );
		testFind( ls, "foo", "bar" );
		testFind( ls, "Configuration", "jan" );
		testFind( ls, "Configuration", null );
		testFind( ls, "Configuration", "scott" );
		testFind( ls, "Authenticator", "jan" );
		testFind( ls, "Authenticator", null );
		testFind( ls, "Authenticator", "scott" );
		testFind( ls, "Log", "louis" );
		testFind( ls, "Locator", "badri" );
		testFind( ls, null, null );
		((RemoteLocatorServer) ls)._stop();
	}
	
	private static void testFind( LocatorServer ls, String service, String instance )
	{
		URL query = new URL( String.format( "etch://%s/tcp/%s", service, instance ) );
		Registration[] ctxs = ls.find( query, 0, Integer.MAX_VALUE );
		System.out.printf( "find %s/%s found %d contexts:\n", service, instance, ctxs.length );
		for (Registration c: ctxs)
			System.out.printf( "  %s/%s: %s@%d\n", c.description, c.contact, c.who, c.registered );
	}
}
