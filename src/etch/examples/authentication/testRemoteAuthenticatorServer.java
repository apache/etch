package etch.examples.authentication;

import etch.examples.authentication.Authenticator.Failure;

/**
 * @author bchampak
 * 
 */
public class testRemoteAuthenticatorServer
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
				System.out.println( "usage: testRemoteAuthenticatorServer [host port instance]" );
				System.exit( 1 );
			}
			host = args[0];
			port = Integer.parseInt( args[1] );
			instance = args[2];
		}
		else
		{
			host = "localhost";
			port = 4002;
			instance = AuthenticatorHelper.LOCAL;
		}

		AuthenticatorServer as =
			AuthenticatorHelper.getAuthenticatorInstance( host, port, instance );

		// login Mary; Mary has check and admin rights
		
		as.login( "Mary", "cisco456" );
		System.out.println( "Mary logged in" );
		
		// test bad user.
		
		assertFalse( as.isAuthentic( "bad", "bad" ) );
		assertFalse( as.hasRight( "bad", "bad" ) );
		System.out.println( "  tested bad user" );
		
		// test good user with bad password and right.
		
		assertFalse( as.isAuthentic( "Fred", "bad" ) );
		assertFalse( as.hasRight( "Fred", "bad" ) );
		System.out.println( "  tested good user with bad password and right" );
		
		// test good user with good password and right.
		
		assertTrue( as.isAuthentic( "Fred", "cisco123" ) );
		assertTrue( as.hasRight( "Fred", "Log.write" ) );
		System.out.println( "  tested good user with good password and right" );
		
		// test an admin operation
		
		String[] rights = as.getRights( "Fred" );
		System.out.println( "  tested an admin operation" );
		for (String right: rights)
			System.out.printf( "  Fred has right %s\n", right );
		System.out.println( "  end of rights list" );
		assertTrue( rights.length == 1 );
		assertTrue( rights[0].equals( "Log.write" ) );
		
		// logout Mary
		
		as.logout();
		System.out.println( "Mary logged out" );
		
		// login Joe; Joe only has check right
		
		as.login( "Joe", "cisco789" );
		System.out.println( "Joe logged in" );
		
		// test bad user.
		
		assertFalse( as.isAuthentic( "bad", "bad" ) );
		assertFalse( as.hasRight( "bad", "bad" ) );
		System.out.println( "  tested bad user" );
		
		// test good user with bad password and right.
		
		assertFalse( as.isAuthentic( "Fred", "bad" ) );
		assertFalse( as.hasRight( "Fred", "bad" ) );
		System.out.println( "  tested good user with bad password and right" );
		
		// test good user with good password and right.
		
		assertTrue( as.isAuthentic( "Fred", "cisco123" ) );
		assertTrue( as.hasRight( "Fred", "Log.write" ) );
		System.out.println( "  tested good user with good password and right" );
		
		// test an admin operation
		
		try
		{
			as.getRights( "Fred" );
			assertFalse( true );
		}
		catch ( Exception e )
		{
			System.out.println( "  Joe was prevented from performing an admin operation: "+e );
			// e.printStackTrace();
		}
		
		as.logout();
		System.out.println( "Joe logged out" );

		// login Fred; Fred has no rights
		
		try
		{
			as.login( "Fred", "cisco123" );
			assertFalse( true );
		}
		catch ( Failure e )
		{
			System.out.println( "Fred was prevented from logging in: "+e );
			// e.printStackTrace();
		}
		
		// we're done, shutdown.

		((RemoteAuthenticatorServer) as)._stop();
	}
	
	private static void assertFalse( boolean value )
	{
		if (value)
			throw new RuntimeException( "expected false, got true" );
	}
	
	private static void assertTrue( boolean value )
	{
		if (!value)
			throw new RuntimeException( "expected true, got false" );
	}
}
