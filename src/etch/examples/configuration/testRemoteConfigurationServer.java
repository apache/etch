package etch.examples.configuration;

import etch.bindings.java.util.StrStrHashMap;

/**
 * @author bchampak
 */
public class testRemoteConfigurationServer
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
				System.out.println( "usage: testRemoteConfigurationServer [host port instance]" );
				System.exit( 1 );
			}
			host = args[0];
			port = Integer.parseInt( args[1] );
			instance = args[2];
		}
		else
		{
			host = "localhost";
			port = 4003;
			instance = ConfigurationHelper.LOCAL;
		}
		
		ConfigurationServer cs =
			ConfigurationHelper.getConfigurationInstance( host, port, instance );
		
		// try some protected operations.
		
		try { cs.getAllConfig( "a" ); assertTrue( false ); } catch ( Exception e ) {}
		System.out.println( "unauthorized getAllConfig prevented" );
		
		try { cs.getConfig( "a", "var1" ); assertTrue( false ); } catch ( Exception e ) {}
		System.out.println( "unauthorized getConfig prevented" );
		
		// try a bad user.
		
		try { cs.login( "bad", "bad" ); assertTrue( false ); } catch ( Exception e ) {}
		assertFalse( cs.isLoggedIn() );
		System.out.println( "bad user login prevented" );
		
		// try a bad password.
		
		try { cs.login( "Fred", "bad" ); assertTrue( false ); } catch ( Exception e ) {}
		assertFalse( cs.isLoggedIn() );
		System.out.println( "bad password login prevented" );
		
		// try a user with no rights.
		
		try { cs.login( "Fred", "cisco123" ); assertTrue( false ); } catch ( Exception e ) {}
		assertFalse( cs.isLoggedIn() );
		System.out.println( "no rights login prevented" );
		
		// try a user with rights.
		
		cs.login( "Joe", "cisco789" );
		assertTrue( cs.isLoggedIn() );
		System.out.println( "Joe login ok" );
		
		StrStrHashMap map = cs.getAllConfig( null );
		assertTrue( map == null );
		System.out.println( "getAllConfig of null ok" );
		
		map = cs.getAllConfig( "bad" );
		assertTrue( map == null );
		System.out.println( "getAllConfig of bad ok" );
		
		map = cs.getAllConfig( "a" );
		assertTrue( map != null );
		assertTrue( map.size() == 2 );
		assertTrue( map.get( "var1" ) != null );
		assertTrue( map.get( "var1" ).equals( "123" ) );
		assertTrue( map.get( "var2" ) != null );
		assertTrue( map.get( "var2" ).equals( "234" ) );
		System.out.println( "getAllConfig of a ok" );
		
		map = cs.getAllConfig( "b" );
		assertTrue( map != null );
		assertTrue( map.size() == 1 );
		assertTrue( map.get( "var1" ) != null );
		assertTrue( map.get( "var1" ).equals( "345" ) );
		System.out.println( "getAllConfig of b ok" );
		
		map = cs.getAllConfig( "c" );
		assertTrue( map != null );
		assertTrue( map.size() == 0 );
		System.out.println( "getAllConfig of c ok" );
		
		try { cs.getConfig( null, null ); assertTrue( false ); } catch ( Exception e ) {}
		System.out.println( "getConfig of null path ok" );
		
		try { cs.getConfig( "bad", null ); assertTrue( false ); } catch ( Exception e ) {}
		System.out.println( "getConfig of bad path ok" );
		
		String value = cs.getConfig( "a", null );
		assertTrue( value == null );
		System.out.println( "getConfig of null name ok" );
		
		value = cs.getConfig( "a", "bad" );
		assertTrue( value == null );
		System.out.println( "getConfig of bad name ok" );
		
		value = cs.getConfig( "a", "var1" );
		assertTrue( value != null );
		assertTrue( value.equals( "123" ) );
		System.out.println( "getConfig of good name ok" );
		
		value = cs.getConfig( "a", "var2" );
		assertTrue( value != null );
		assertTrue( value.equals( "234" ) );
		System.out.println( "getConfig of good name ok" );
		
		value = cs.getConfig( "b", null );
		assertTrue( value == null );
		System.out.println( "getConfig of null name ok" );
		
		value = cs.getConfig( "b", "bad" );
		assertTrue( value == null );
		System.out.println( "getConfig of bad name ok" );
		
		value = cs.getConfig( "b", "var1" );
		assertTrue( value != null );
		assertTrue( value.equals( "345" ) );
		System.out.println( "getConfig of good name ok" );
		
		value = cs.getConfig( "c", null );
		assertTrue( value == null );
		System.out.println( "getConfig of null name ok" );
		
		value = cs.getConfig( "c", "bad" );
		assertTrue( value == null );
		System.out.println( "getConfig of bad name ok" );
		
		((RemoteConfigurationServer) cs)._stop();
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
