package org.apache.etch.services.config;

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
	public void root1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// test properties on the root node
		
		Assert.assertNull( c.getParent( r ) );
		Assert.assertEquals( "", c.getName( r ) );
		Assert.assertNull( c.getIndex( r ) );
		Assert.assertEquals( "/", c.getPath( r ) );
		Assert.assertTrue( c.isRoot( r ) );
		Assert.assertFalse( c.isList( r ) );
		Assert.assertTrue( c.isMap( r ) );
		Assert.assertEquals( 7, c.size( r ) );

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
	
	private static class MyConfigurationClient implements ConfigurationClient
	{
		public void configValuesChanged( Object[] updated )
		{
			throw new UnsupportedOperationException( "configValuesChanged" );
		}
	}
}
