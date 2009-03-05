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
	public void getParent1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		// these are euphemisms for the root node
		Assert.assertNull( c.getParent( r ) );
		Assert.assertNull( c.getParent( c.getConfigPath( r, "" ) ) );
		Assert.assertNull( c.getParent( c.getConfigPath( r, "." ) ) );
		Assert.assertNull( c.getParent( c.getConfigPath( r, "/" ) ) );
		Assert.assertNull( c.getParent( c.getConfigPath( null, "/" ) ) );

		Assert.assertEquals( r,
			c.getParent( c.getConfigPath( null, "/bool" ) ) );
		Assert.assertEquals( r,
			c.getParent( c.getConfigPath( r, "bool" ) ) );
		
		Assert.assertEquals( c.getConfigPath( null, "users" ),
			c.getParent( c.getConfigPath( null, "users/fred" ) ) );
	}
	
	private static class MyConfigurationClient implements ConfigurationClient
	{
		public void configValuesChanged( Object[] updated )
		{
			throw new UnsupportedOperationException( "configValuesChanged" );
		}
	}
}
