package org.apache.etch.services.config;

import org.apache.etch.services.config.Configuration.ConfigurationException;
import org.junit.Assert;
import org.junit.Test;

/** Test of YamlConfig */
public class TestYamlConfig
{
	private static final String REMOTE = "services/config/remote";
	
	private static final String BOGUS = "services/config/bogus";
	
	private static final String DIR = "services/config/dir";
	
	private final MyConfigurationClient client = new MyConfigurationClient();
	
	/** @throws Exception */
	@Test
	public void construct1() throws Exception
	{
		new YamlConfig( null );
	}
	
	/** @throws Exception */
	@Test
	public void construct2() throws Exception
	{
		new YamlConfig( client );
	}
	
	/** @throws Exception */
	@Test
	public void construct3() throws Exception
	{
		new YamlConfig( null, null );
	}
	
	/** @throws Exception */
	@Test
	public void construct4() throws Exception
	{
		new YamlConfig( null, REMOTE );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct4a() throws Exception
	{
		new YamlConfig( null, "" );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct4b() throws Exception
	{
		new YamlConfig( null, DIR );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct4c() throws Exception
	{
		new YamlConfig( null, BOGUS );
	}
	
	/** @throws Exception */
	@Test
	public void construct5() throws Exception
	{
		new YamlConfig( client, null );
	}
	
	/** @throws Exception */
	@Test
	public void construct6() throws Exception
	{
		new YamlConfig( client, REMOTE );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct6a() throws Exception
	{
		new YamlConfig( client, "" );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct6b() throws Exception
	{
		new YamlConfig( client, DIR );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void construct6c() throws Exception
	{
		new YamlConfig( client, BOGUS );
	}
	
	/** @throws Exception */
	@Test
	public void canload() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		Assert.assertFalse( c.canLoad( null ) );
		Assert.assertFalse( c.canLoad( "" ) );
		Assert.assertFalse( c.canLoad( DIR ) );
		Assert.assertFalse( c.canLoad( BOGUS ) );
		Assert.assertTrue( c.canLoad( REMOTE ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void notloaded1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.getPath( null );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void notloaded2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, null );
		c.getPath( null );
	}
	
	/** @throws Exception */
	@Test( expected = ConfigurationException.class)
	public void notloaded3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, null );
		c.loadConfig( BOGUS );
	}
	
	/** @throws Exception */
	@Test
	public void notloaded4() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, null );
		try { c.loadConfig( BOGUS ); } catch ( ConfigurationException e ) {}
		Assert.assertFalse( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test
	public void notloaded5() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, null );
		
		c.loadConfig( REMOTE );
		Assert.assertTrue( c.isLoaded() );
		try { c.loadConfig( null ); } catch ( ConfigurationException e ) {}
		Assert.assertFalse( c.isLoaded() );
		
		c.loadConfig( REMOTE );
		Assert.assertTrue( c.isLoaded() );
		try { c.loadConfig( "" ); } catch ( ConfigurationException e ) {}
		Assert.assertFalse( c.isLoaded() );
		
		c.loadConfig( REMOTE );
		Assert.assertTrue( c.isLoaded() );
		try { c.loadConfig( DIR ); } catch ( ConfigurationException e ) {}
		Assert.assertFalse( c.isLoaded() );
		
		c.loadConfig( REMOTE );
		Assert.assertTrue( c.isLoaded() );
		try { c.loadConfig( BOGUS ); } catch ( ConfigurationException e ) {}
		Assert.assertFalse( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test
	public void loaded1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.loadConfig( REMOTE );
		Assert.assertEquals( "", c.getPath( null ) );
	}
	
	/** @throws Exception */
	@Test
	public void loaded2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Assert.assertEquals( "", c.getPath( null ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void unloaded1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.loadConfig( REMOTE );
		Assert.assertEquals( "", c.getPath( null ) );
		c.unloadConfig();
		c.getPath( null );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void unloaded2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Assert.assertEquals( "", c.getPath( null ) );
		c.unloadConfig();
		c.getPath( null );
	}
	
	/** @throws Exception */
	@Test
	public void isloaded1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		Assert.assertFalse( c.isLoaded() );
		c.loadConfig( REMOTE );
		Assert.assertTrue( c.isLoaded() );
		c.unloadConfig();
		Assert.assertFalse( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test
	public void isloaded2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		Assert.assertTrue( c.isLoaded() );
		c.unloadConfig();
		Assert.assertFalse( c.isLoaded() );
	}
	
	/** @throws Exception */
	@Test
	public void getParent1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null, REMOTE );
		
		Object root = c.getConfigPath( null, null );
		Assert.assertNotNull( root );
		
		// these are euphemisms for the root node
		Assert.assertNull( c.getParent( null ) );
		Assert.assertNull( c.getParent( c.getConfigPath( null, null ) ) );
		Assert.assertNull( c.getParent( c.getConfigPath( null, "" ) ) );
		Assert.assertNull( c.getParent( c.getConfigPath( null, "." ) ) );
		Assert.assertNull( c.getParent( c.getConfigPath( null, "/" ) ) );
		
		Assert.assertEquals( c.getConfigPath( null, null ),
			c.getParent( c.getConfigPath( null, "bool" ) ) );
		
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
