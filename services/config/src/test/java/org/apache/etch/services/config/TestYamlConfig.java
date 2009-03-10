package org.apache.etch.services.config;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;

import org.apache.etch.services.config.Configuration.ConfigurationException;
import org.junit.Assert;
import org.junit.Test;

/** Test of YamlConfig */
public class TestYamlConfig
{
	private static final String REMOTE = "services/config/remote";
	
	private static final String BOGUS = "services/config/bogus";
	
	private static final String BLANK = "";
	
	private static final String EMPTY = "services/config/empty";
	
	private static final String DIR = "services/config/dir";
	
	private final MyConfigurationClient client = new MyConfigurationClient();
	
	/** @throws Exception */
	@Test
	public void construct1() throws Exception
	{
		YamlConfig c = new YamlConfig( null );
		Assert.assertFalse( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test
	public void construct2() throws Exception
	{
		YamlConfig c = new YamlConfig( client );
		Assert.assertFalse( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test
	public void construct3() throws Exception
	{
		YamlConfig c = new YamlConfig( null, null );
		Assert.assertFalse( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test
	public void construct4() throws Exception
	{
		YamlConfig c = new YamlConfig( client, null );
		Assert.assertFalse( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test
	public void construct5() throws Exception
	{
		YamlConfig c = new YamlConfig( null, REMOTE );
		Assert.assertTrue( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test
	public void construct6() throws Exception
	{
		YamlConfig c = new YamlConfig( client, REMOTE );
		Assert.assertTrue( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct7() throws Exception
	{
		new YamlConfig( null, BLANK );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct8() throws Exception
	{
		new YamlConfig( client, BLANK );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct9() throws Exception
	{
		new YamlConfig( null, EMPTY );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct10() throws Exception
	{
		new YamlConfig( client, EMPTY );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct11() throws Exception
	{
		new YamlConfig( null, DIR );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct12() throws Exception
	{
		new YamlConfig( client, DIR );
	}
	
	/** @throws Exception */
	@Test
	public void canload() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		Assert.assertFalse( c.canLoad( null ) );
		Assert.assertTrue( c.canLoad( REMOTE ) );
		Assert.assertFalse( c.canLoad( BOGUS ) );
		Assert.assertFalse( c.canLoad( BLANK ) );
		Assert.assertTrue( c.canLoad( EMPTY ) );
		Assert.assertFalse( c.canLoad( DIR ) );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void loadConfig1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.loadConfig( null );
	}
	
	/** @throws Exception */
	@Test
	public void loadConfig2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.loadConfig( REMOTE );
		Assert.assertTrue( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void loadConfig3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.loadConfig( BOGUS );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void loadConfig4() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.loadConfig( BLANK );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void loadConfig5() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.loadConfig( EMPTY );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void loadConfig6() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.loadConfig( DIR );
	}
	
	/** @throws Exception */
	@Test
	public void isLoaded() throws Exception
	{
		testLoadConfig( null, false );
		testLoadConfig( REMOTE, true );
		testLoadConfig( BOGUS, false );
		testLoadConfig( BLANK, false );
		testLoadConfig( EMPTY, false );
		testLoadConfig( DIR, false );
	}
	
	private void testLoadConfig( String name, boolean expected )
		throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		Assert.assertFalse( c.isLoaded() );
		try { c.loadConfig( name ); } catch ( Exception e ) {}
		Assert.assertEquals( expected, c.isLoaded() );
		c.unloadConfig();
		Assert.assertFalse( c.isLoaded() );
	}

	/** @throws Exception */
	@Test
	public void unloadConfig1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, null );
		Assert.assertFalse( c.isLoaded() );
		c.unloadConfig();
		Assert.assertFalse( c.isLoaded() );
	}

	/** @throws Exception */
	@Test
	public void unloadConfig2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, null );
		Assert.assertFalse( c.isLoaded() );
		c.unloadConfig();
		Assert.assertFalse( c.isLoaded() );
		c.unloadConfig();
		Assert.assertFalse( c.isLoaded() );
	}

	/** @throws Exception */
	@Test
	public void unloadConfig3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, null );
		Assert.assertFalse( c.isLoaded() );
		c.loadConfig( REMOTE );
		Assert.assertTrue( c.isLoaded() );
		c.unloadConfig();
		Assert.assertFalse( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test
	public void props1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// test properties on the root node
		
		testProps( c, r, null, "", null, "/", true, false, true, 15 );
				
		// test properties on the users node
		
		testProps( c, c.getConfigPath( r, "users" ), r, "users", null, "/users",
			false, false, true, 4 );
				
		// test properties on the primes node
		
		testProps( c, c.getConfigPath( r, "primes" ), r, "primes", null,
			"/primes", false, true, false, 7 );
				
		// test properties on the primes/4 node
		
		testProps( c, c.getConfigPath( r, "primes/4" ),
			c.getConfigPath( r, "primes" ), "4", 4, "/primes/4", false, false,
			false, null );
	}
	
	/** @throws Exception */
	@Test( expected = NoSuchElementException.class )
	public void props2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// value is not a List or a Map
		c.size( c.getConfigPath( r, "primes/4" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void props3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		// id == null
		c.size( null );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void props4() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		// id == null
		c.size( "#" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void props5() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		// id == null
		c.size( "#0" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void props6() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		// id == null
		c.size( "#0#" );
	}
	
	private void testProps( ConfigurationServer c, Object node, Object parent,
		String name, Integer index, String path, boolean isRoot, boolean isList,
		boolean isMap, Integer size )
	{
		Assert.assertEquals( parent, c.getParent( node ) );
		Assert.assertEquals( name, c.getName( node ) );
		Assert.assertEquals( index, c.getIndex( node ) );
		Assert.assertEquals( path, c.getPath( node ) );
		Assert.assertEquals( isRoot, c.isRoot( node ) );
		Assert.assertEquals( isList, c.isList( node ) );
		Assert.assertEquals( isMap, c.isMap( node ) );
		if (size != null)
			Assert.assertEquals( size, c.size( node ) );
	}

	/** @throws Exception */
	@Test
	public void root1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		// try absolute paths for root
		
		Assert.assertEquals( r, c.getConfigPath( null, "/" ) ); // root
		Assert.assertEquals( r, c.getConfigPath( -1, "/" ) ); // root
		Assert.assertEquals( r, c.getConfigPath( "abc", "/" ) ); // root
		Assert.assertEquals( r, c.getConfigPath( false, "/" ) ); // root
		Assert.assertEquals( r, c.getConfigPath( r, "/" ) ); // root
		
		// try relative paths for root

		Assert.assertEquals( r, c.getConfigPath( r, "" ) ); // root
		Assert.assertEquals( r, c.getConfigPath( r, "." ) ); // root
		Assert.assertEquals( r, c.getConfigPath( r, "./." ) ); // root
		Assert.assertEquals( r, c.getConfigPath( r, "././." ) ); // root
		Assert.assertEquals( r, c.getConfigPath( r, "bool/.." ) ); // root
		
		// other relative paths
		
		Assert.assertNull( c.getConfigPath( r, ".." ) ); // parent
		Assert.assertNotNull( c.getConfigPath( r, "bool" ) ); // child
		
		// bad paths

		Assert.assertNull( c.getConfigPath( r, "blah" ) );
		Assert.assertNull( c.getConfigPath( r, "blah/blah" ) );
		Assert.assertNotNull( c.getConfigPath( r, "primes/4" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void root2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// path == null
		c.getConfigPath( r, null );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void root3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// path must be absolute when id == null
		c.getConfigPath( null, "foo" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void root4() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// id is not valid
		c.getConfigPath( -1, "foo" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void root5() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// id is not valid
		c.getConfigPath( 0, "foo" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void root6() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// id is not valid
		c.getConfigPath( 99, "foo" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void root7() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// id is not valid
		c.getConfigPath( "abc", "foo" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void root8() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// id is not valid
		c.getConfigPath( "#abc#", "foo" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void root9() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// id is not valid
		c.getConfigPath( "#?#", "foo" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void root10() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// id is not valid
		c.getConfigPath( false, "foo" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void root11() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// i < 0 || i >= list.size()
		c.getConfigPath( r, "primes/12" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void root12() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.getRoot();
	}
	
	/** @throws Exception */
	@Test( expected = NoSuchElementException.class )
	public void root13() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// abc
		c.getConfigPath( r, "primes/4/abc" );
	}
	
	/** @throws Exception */
	@Test
	public void hasValuePath() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		Assert.assertTrue( c.hasValuePath( r, "." ) );
		Assert.assertTrue( c.hasValuePath( r, "bool" ) );
		Assert.assertFalse( c.hasValuePath( r, "blah" ) );
	}
	
	/** @throws Exception */
	@Test
	public void getValuePath() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		Assert.assertNotNull( c.getValuePath( r, "." ) );
		Assert.assertNotNull( c.getValuePath( r, "bool" ) );
		Assert.assertNull( c.getValuePath( r, "blah" ) );
	}
	
	/** @throws Exception */
	@Test
	public void getBooleanPath1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		Assert.assertEquals( true, c.getBooleanPath( r, "bool" ) );
		Assert.assertEquals( false, c.getBooleanPath( r, "bool2" ) );
		Assert.assertNull( c.getBooleanPath( r, "blah" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getBooleanPath2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		// cannot convert value to Boolean
		c.getBooleanPath( r, "." );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getBooleanPath3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to Boolean
		c.getBooleanPath( r, "int" );
	}
	
	/** @throws Exception */
	@Test
	public void getIntegerPath1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		Assert.assertEquals( 23, c.getIntegerPath( r, "int" ) );
		Assert.assertEquals( 34, c.getIntegerPath( r, "int2" ) );
		Assert.assertNull( c.getIntegerPath( r, "blah" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getIntegerPath2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		// cannot convert value to Integer
		c.getIntegerPath( r, "." );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getIntegerPath3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to Integer
		c.getIntegerPath( r, "bool" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getIntegerPath4() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to Integer
		c.getIntegerPath( r, "int3" );
	}
	
	/** @throws Exception */
	@Test
	public void getDoublePath1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		Assert.assertEquals( 4.5, c.getDoublePath( r, "dbl" ) );
		Assert.assertEquals( 5.6, c.getDoublePath( r, "dbl2" ) );
		Assert.assertEquals( 67, c.getDoublePath( r, "dbl3" ) );
		Assert.assertNull( c.getDoublePath( r, "blah" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getDoublePath2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		// cannot convert value to Double
		c.getDoublePath( r, "." );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getDoublePath3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to Double
		c.getDoublePath( r, "bool" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getDoublePath4() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to Double
		c.getDoublePath( r, "dbl4" );
	}
	
	/** @throws Exception */
	@SuppressWarnings("deprecation")
	@Test
	public void getDatePath1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		Assert.assertEquals( new Date( "12/19/2008" ), c.getDatePath( r, "date" ) );
		Assert.assertEquals( new Date( "12/23/2008" ), c.getDatePath( r, "date2" ) );
		Assert.assertNull( c.getDatePath( r, "blah" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getDatePath2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		// cannot convert value to Date
		c.getDatePath( r, "." );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getDatePath3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to Date
		c.getDatePath( r, "bool" );
	}
	
	/** @throws Exception */
	@Test
	public void getStringPath1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		Assert.assertEquals( "true", c.getStringPath( r, "bool" ) );
		Assert.assertEquals( "23", c.getStringPath( r, "int" ) );
		Assert.assertEquals( "4.5", c.getStringPath( r, "dbl" ) );
		Assert.assertEquals( "boo", c.getStringPath( r, "str" ) );
		Assert.assertEquals( "12/19/2008", c.getStringPath( r, "date" ) );
		Assert.assertEquals( "Tue Dec 23 00:00:00 CST 2008", c.getStringPath( r, "date2" ) );
		Assert.assertNull( c.getStringPath( r, "blah" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getStringPath2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		// cannot convert value to String
		c.getStringPath( r, "." );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getStringPath3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to String
		c.getStringPath( r, "primes" );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getStringPath4() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to String
		c.getStringPath( r, "users" );
	}
	
	/** @throws Exception */
	@Test
	public void getListPath1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		List<Integer> l = new ArrayList<Integer>();
		l.add( 1 );
		l.add( 2 );
		l.add( 3 );
		l.add( 5 );
		l.add( 7 );
		l.add( 11 );
		l.add( 13 );

		Assert.assertEquals( l, c.getListPath( r, "primes", null ) );
		Assert.assertEquals( l, c.getListPath( r, "primes", 1 ) );
		Assert.assertNull( c.getListPath( r, "primes", 0 ) );
		Assert.assertNull( c.getListPath( r, "blah", null ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getListPath2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to List
		c.getListPath( r, "users", null );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getListPath3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to List
		c.getListPath( r, "int", null );
	}
	
	/** @throws Exception */
	@Test
	public void getMapPath1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Map<String, Object> m = new HashMap<String, Object>();
		m.put( "fred", 1234 );
		m.put( "mary", 2345 );
		m.put( "alice", 9876 );
		m.put( "jack", 8765 );
		
		Assert.assertEquals( m, c.getMapPath( r, "users", null ) );
		Assert.assertEquals( m, c.getMapPath( r, "users", 1 ) );
		Assert.assertNull( c.getMapPath( r, "users", 0 ) );
		Assert.assertNull( c.getMapPath( r, "blah", null ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getMapPath2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to Map
		c.getMapPath( r, "primes", null );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getMapPath3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// cannot convert value to Map
		c.getMapPath( r, "int", null );
	}
	
	/** @throws Exception */
	@Test
	public void getConfigIndex1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object p = c.getConfigPath( r, "primes" );
		Assert.assertEquals( 7, c.getInteger( c.getConfigIndex( p, 4 ) ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getConfigIndex2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object p = c.getConfigPath( r, "primes" );
		c.getConfigIndex( p, -1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getConfigIndex3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object p = c.getConfigPath( r, "primes" );
		c.getConfigIndex( p, 99 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getConfigIndex4() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object p = c.getConfigPath( r, "primes" );
		c.getConfigIndex( p, null );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getConfigIndex5() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		c.getConfigIndex( r, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void getConfigIndex6() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		c.getConfigIndex( null, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void listConfigPathIds1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object[] ids = c.listConfigPathIds( r, "users", null, null );
		Assert.assertEquals( 4, ids.length );
	}
	
	/** @throws Exception */
	@Test
	public void listConfigPathIds2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object[] ids = c.listConfigPathIds( r, "primes", null, null );
		Assert.assertEquals( 7, ids.length );
	}
	
	/** @throws Exception */
	@Test( expected = NoSuchElementException.class )
	public void listConfigPathIds3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// value is not a List or a Map
		c.listConfigPathIds( r, "primes/4", null, null );
	}
	
	/** @throws Exception */
	@Test
	public void listConfigPathIds4() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object[] ids = c.listConfigPathIds( r, "primes", 1, 3 );
		Assert.assertEquals( 3, ids.length );
	}
	
	/** @throws Exception */
	@Test
	public void listConfigPathIds5() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object[] ids = c.listConfigPathIds( r, "primes", 5, 4 );
		Assert.assertEquals( 2, ids.length );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void listConfigPathIds6() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// offset < 0
		c.listConfigPathIds( r, "primes", -1, 4 );
	}
	
	/** @throws Exception */
	@Test
	public void listConfigPathIds7() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object[] ids = c.listConfigPathIds( r, "primes", 7, 4 );
		Assert.assertEquals( 0, ids.length );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void listConfigPathIds8() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// offset > size
		c.listConfigPathIds( r, "primes", 8, 4 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void listConfigPathIds9() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// count < 0
		c.listConfigPathIds( r, "primes", 0, -1 );
	}
	
	private static class MyConfigurationClient implements ConfigurationClient
	{
		public void configValuesChanged( Object[] updated )
		{
			throw new UnsupportedOperationException( "configValuesChanged" );
		}
	}
}
