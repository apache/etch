/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.services.config;

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Set;

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

	private static final String UPDATE = "services/config/update";
	
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
	
	/** @throws Exception */
	@Test
	public void subscribePath1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( client, REMOTE );
		client.setServer( c );
		
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		c.subscribePath( r, "users" );
		c.subscribePath( r, "users" );
		
		c.subscribePath( r, "primes" );
		c.subscribePath( r, "primes" );
	}
	
	/** @throws Exception */
	@Test
	public void subscribePath2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( client, REMOTE );
		client.setServer( c );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		c.subscribePath( r, "blah" );
	}
	
	/** @throws Exception */
	@Test
	public void unsubscribePath1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( client, REMOTE );
		client.setServer( c );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		c.subscribePath( r, "users" );
		c.unsubscribePath( r, "users" );
		
		c.subscribePath( r, "primes" );
		c.unsubscribePath( r, "primes" );
	}
	
	/** @throws Exception */
	@Test
	public void unsubscribePath2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( client, REMOTE );
		client.setServer( c );
		Object r = c.getRoot();
		Assert.assertNotNull( r );

		c.unsubscribePath( r, "blah" );
	}
	
	/** @throws Exception */
	@Test
	public void configValuesChanged1() throws Exception
	{
		ConfigurationServer c = new YamlConfig( client, REMOTE );
		client.setServer( c );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Assert.assertEquals( set(), client.changed );
		
		c.subscribe( r );
		Thread.sleep( 100 );
		Assert.assertEquals( set( r ), client.changed );
	}
	
	/** @throws Exception */
	@Test
	public void configValuesChanged2() throws Exception
	{
		ConfigurationServer c = new YamlConfig( client, REMOTE );
		client.setServer( c );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object x = c.getConfigPath( r, "users" );
		Assert.assertNotNull( x );
		
		Assert.assertEquals( set(), client.changed );
		
		c.subscribe( x );
		Thread.sleep( 100 );
		Assert.assertEquals( set( x ), client.changed );
	}
	
	/** @throws Exception */
	@Test
	public void configValuesChanged3() throws Exception
	{
		ConfigurationServer c = new YamlConfig( client, REMOTE );
		client.setServer( c );
		Object r = c.getRoot();
		Assert.assertNotNull( r );
		
		Object x = c.getConfigPath( r, "users" );
		Assert.assertNotNull( x );
		
		Object y = c.getConfigPath( r, "primes" );
		Assert.assertNotNull( y );

		Assert.assertEquals( set(), client.changed );

		c.subscribe( x );
		Thread.sleep( 100 );
		Assert.assertEquals( set( x ), client.changed );

		c.subscribe( y );
		Thread.sleep( 100 );
		Assert.assertEquals( set( y ), client.changed );
	}
	
	/** @throws Exception */
	@Test
	public void update0() throws Exception
	{
		YamlConfig c = new YamlConfig( client, UPDATE+0 );
		client.setServer( c );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update1() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+1 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 3, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update2() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+2 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 6, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( 3, c.getIntegerPath( root, "baz" ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update3() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+3 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 4, c.size( root ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update4() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+4 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 5 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update5() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+5 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4, 5 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update6() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+6 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update7() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+7 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 6, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( map( "x", 6, "y", 7, "z", 8 ), c.getMapPath( root, "map1", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update8() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+8 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 6, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( 3, 4, 5 ), c.getListPath( root, "array1", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update9() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+9 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 4, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update10() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+10 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 4, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update11() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+11 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), map( "d", 8, "e", 9, "f", 10 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update12() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+12 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), list( 4, 5, 6 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update13() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+13 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update14() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+14 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update15() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+15 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( map( "x", 4, "y", 5, "z", 6 ), c.getMapPath( root, "bar", null ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update16() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+16 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( list( 4, 5, 6 ), c.getListPath( root, "bar", null ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update17() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+17 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( 17, c.getIntegerPath( root, "map" ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update18() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+18 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "x", 8, "y", 9, "z", 10 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update19() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+19 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( list( 3, 4, 5 ), c.getListPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update20() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+20 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( 17, c.getIntegerPath( root, "array" ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update21() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+21 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( map( "i", 9, "j", 8 ), c.getMapPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update22() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+22 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 4, 5, 6 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update23() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+23 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), map( "x", 8, "y", 7, "z", 6 ) ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update24() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+24 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 1, 2, 3 ), list( 2, 3, 4 ) ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update25() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+25 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", 25, list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update26() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+26 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "x", 6, "y", 7, "z", 8 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update27() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+27 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", list( 2, 3, 4 ), list( 1, 2, 3 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update28() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+28 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), 34, "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update29() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+29 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), map( "x", 7, "y", 8, "z", 9 ), "bear" ), c.getListPath( root, "list", null ) );
	}

	/** @throws Exception */
	@Test
	public void update30() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+30 );
		Object root = c.getRoot();
		
		Assert.assertEquals( 5, c.size( root ) );
		Assert.assertEquals( 2, c.getIntegerPath( root, "foo" ) );
		Assert.assertEquals( 4, c.getIntegerPath( root, "bar" ) );
		Assert.assertEquals( list( 2, 3, 4 ), c.getListPath( root, "array", null ) );
		Assert.assertEquals( map( "a", 5, "b", 6, "c", 7 ), c.getMapPath( root, "map", null ) );
		Assert.assertEquals( list( "fish", map( "a", 5, "b", 6, "c", 7 ), list( 2, 3, 4 ), "bear" ), c.getListPath( root, "list", null ) );
	}
	
	/** @throws Exception */
	@Test
	public void subscribeUpdate0() throws Exception
	{
		setupUpdate( UPDATE+0, "." );
		Assert.assertEquals( set(), client.changed );
	}
	
	/** @throws Exception */
	@Test
	public void subscribeUpdate1a() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+1, "." );
		Assert.assertEquals( set( c.getConfigPath( c.getRoot(), "." ) ), client.changed );
	}
	
	/** @throws Exception */
	@Test
	public void subscribeUpdate1b() throws Exception
	{
		YamlConfig c = setupUpdate( UPDATE+1, "foo" );
		Assert.assertEquals( set( c.getConfigPath( c.getRoot(), "foo" ) ), client.changed );
	}
	
	/** @throws Exception */
	@Test
	public void subscribeUpdate1c() throws Exception
	{
		setupUpdate( UPDATE+1, "bar" );
		Assert.assertEquals( set(), client.changed );
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
	
	private YamlConfig setupUpdate( String updateConfig, String path ) throws Exception
	{
		YamlConfig c = new YamlConfig( client, UPDATE+0 );
		client.setServer( c );
		Object root = c.getRoot();
		
		c.setInterval( 50 );
		c.subscribePath( root, path );
		c.setConfig( updateConfig );
		Thread.sleep( 1000 );
		c.unsubscribeAll();
		
		return c;
	}
	
	private YamlConfig setupUpdate( String updateConfig ) throws Exception
	{
		return setupUpdate( updateConfig, "." );
	}
	
	private List<?> list( Object ... objs )
	{
		List<Object> list = new ArrayList<Object>( objs.length );
		for (Object obj: objs)
			list.add( obj );
		return list;
	}
	
	private Map<?, ?> map( Object ... objs )
	{
		Map<String, Object> map = new HashMap<String, Object>(
			(objs.length * 2 + 2) / 3 );
		for (int i = 0; i < objs.length; i += 2)
			map.put( (String) objs[i], objs[i + 1] );
		return map;
	}
	
	private Set<?> set( Object ... objs )
	{
		Set<Object> set = new HashSet<Object>();
		for (Object o: objs)
			set.add( o );
		return set;
	}
	
	private static class MyConfigurationClient implements ConfigurationClient
	{
		public void configValuesChanged( Object[] updated )
		{
			Set<Object> changed = new HashSet<Object>();
			for (Object id: updated)
			{
				System.out.println( "id changed: "+id );
				System.out.println( "path changed: "+server.getPath( id ) );
				changed.add( id );
			}
			this.changed = changed;
		}
		
		private Set<Object> changed = new HashSet<Object>();

		public void setServer( ConfigurationServer server )
		{
			this.server = server;
		}
		
		private ConfigurationServer server;
	}
}
