package org.apache.etch.services.config;

import org.apache.etch.services.config.Configuration.ConfigurationException;
import org.junit.Assert;
import org.junit.Test;

/** Test of YamlConfig */
public class TestYamlConfig
{
	private static final String REMOTE = "services/config/remote";
	
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
	@Test( expected = IllegalArgumentException.class )
	public void notloaded1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.getPath( null );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void notloaded2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( client );
		c.getPath( null );
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
		ConfigurationServer c = new YamlConfig( client, REMOTE );
		Assert.assertEquals( "", c.getPath( null ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void unloaded1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( null );
		c.loadConfig( REMOTE );
		Assert.assertEquals( "", c.getPath( null ) );
		c.unloadConfig();
		c.getPath( null );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void unloaded2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( client, REMOTE );
		Assert.assertEquals( "", c.getPath( null ) );
		c.unloadConfig();
		c.getPath( null );
	}
	
	private static class MyConfigurationClient implements ConfigurationClient
	{
		public void configValuesChanged( Object[] updated )
		{
			// TODO Auto-generated method stub
		}
	}
}
