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

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.StringTokenizer;

import org.apache.etch.util.AlarmListener;
import org.apache.etch.util.AlarmManager;
import org.apache.etch.util.Assertion;
import org.apache.etch.util.IdGenerator;
import org.apache.etch.util.Todo;
import org.apache.etch.util.TodoManager;
import org.ho.yaml.Yaml;
import org.ho.yaml.exception.YamlException;


/** Local Yaml implementation of ConfigurationServer */
public class YamlConfig implements ConfigurationServer
{
	/**
	 * Constructs an empty YamlConfigurationServer.
	 * @param client 
	 * @throws ConfigurationException 
	 */
	public YamlConfig( ConfigurationClient client )
		throws ConfigurationException
	{
		this( client, null );
	}
	
	/**
	 * Constructs a YamlConfigurationServer, then loads the config.
	 * @param client 
	 * @param name
	 * @throws ConfigurationException
	 */
	public YamlConfig( ConfigurationClient client, String name )
		throws ConfigurationException
	{
		this.client = client;
		if (name == null)
			return;
		loadConfig( name );
	}
	
	private final ConfigurationClient client;

	////////////////////
	// PUBLIC METHODS //
	////////////////////

	public Object loadConfig( String name )
		throws ConfigurationException
	{
		unloadConfig();
		
		File f = mkfile( name );
		
		for (int i = 0; i < 4; i++)
		{
			long t0 = f.lastModified();
			Object o = loadConfig0( f );
			long t1 = f.lastModified();
			
			if (t1 == t0)
			{
				configs = importConfigs( o );
				file = f;
				lastModified = t0;
				subs = Collections.synchronizedSet( new HashSet<Integer>() );
				return getRoot();
			}
			
			try
			{
				Thread.sleep( 1000 );
			}
			catch ( InterruptedException e )
			{
				break;
			}
		}
		
		throw new ConfigurationException( "config is changing: "+name );
	}

	public void unloadConfig()
	{
		unsubscribeAll();
		file = null;
		lastModified = null;
		configs = null;
		subs = null;
	}

	public Boolean canLoad( String name )
	{
		try
		{
			mkfile( name );
			return true;
		}
		catch ( ConfigurationException e )
		{
			return false;
		}
	}

	public Boolean isLoaded()
	{
		return configs != null;
	}

	public Object getRoot()
	{
		if (!isLoaded())
			throw new IllegalStateException( "no config loaded" );
		
		if (configs.size() == 0)
			throw new IllegalStateException( "config is empty" );
		
		return toId( 0 );
	}
	
	/////////////////////
	// NODE PROPERTIES //
	/////////////////////

	public Object getParent( Object id )
	{
		return toId( getConf( id ).parent );
	}

	public String getName( Object id )
	{
		return getConf( id ).name();
	}

	public Integer getIndex( Object id )
	{
		return getConf( id ).index();
	}

	public String getPath( Object id )
	{
		return getConf( id ).getPath();
	}

	public Boolean isRoot( Object id )
	{
		return getConf( id ).isRoot();
	}

	public Boolean isList( Object id )
	{
		return getConf( id ).isList();
	}

	public Boolean isMap( Object id )
	{
		return getConf( id ).isMap();
	}
	
	public Integer size( Object id )
	{
		return getConf( id ).size();
	}
	
	//////////////
	// CHILDREN //
	//////////////

	public Object[] listConfigIds( Object id, Integer offset, Integer count )
	{
		Conf c = getConf( id );
		
		if (c.isList())
			return toIdArray( c.list(), offset, count );
		
		if (c.isMap())
			return toIdArray( c.map().values(), offset, count );
		
		throw new NoSuchElementException( "value is not a List or a Map" );
	}
	
	public Object[] listConfigPathIds( Object id, String path, Integer offset,
		Integer count )
	{
		return listConfigIds( getConfigPath( id, path ), offset, count );
	}

	public Object getConfigIndex( Object id, Integer index )
	{
		if (index == null)
			throw new IllegalArgumentException( "index == null" );
		
		Conf c = getConf( id );
		
		if (c.isList())
		{
			List<Integer> list = c.list();
			
			if (index < 0 || index >= list.size())
				throw new IllegalArgumentException(
					"index < 0 || index >= list.size()" );
			
			return toId( c.list().get( index ) );
		}
		
		throw new IllegalArgumentException( "value is not a List" );
	}

	public Object getConfigPath( Object id, String path )
	{
		checkPath( path );
		// path is not null.
		
		// id == null: path must be absolute.
		// id != null: path may be relative or absolute.
		
		if (id == null)
		{
			if (!isAbsolute( path ))
				throw new IllegalArgumentException(
					"path must be absolute when id == null" );
			
			id = getRoot();
			path = abs2rel( path );
		}
		else
		{
			if (isAbsolute( path ))
			{
				id = getRoot();
				path = abs2rel( path );
			}
			else
			{
				getConf( id );
			}
		}
		
		// id is not null
		
		Integer iid = fromId( id );
		
		// if we have a simple path just let getConfig0 handle it
		
		if (path.indexOf( '/' ) < 0)
			return toId( getConfigName( iid, path ) );
		
		// complex path
		
		StringTokenizer st = new StringTokenizer( path, "/" );
		while (iid != null && st.hasMoreTokens())
			iid = getConfigName( iid, st.nextToken() );
		
		return toId( iid );
	}

	////////////////////////
	// NODE / PATH ACCESS //
	////////////////////////

	public Boolean hasValuePath( Object id, String path )
	{
		return hasValue( getConfigPath( id, path ) );
	}

	public Object getValuePath( Object id, String path )
	{
		return getValue( getConfigPath( id, path ) );
	}

	public Boolean getBooleanPath( Object id, String path )
	{
		return getBoolean( getConfigPath( id, path ) );
	}

	public Integer getIntegerPath( Object id, String path )
	{
		return getInteger( getConfigPath( id, path ) );
	}

	public Double getDoublePath( Object id, String path )
	{
		return getDouble( getConfigPath( id, path ) );
	}

	public String getStringPath( Object id, String path )
	{
		return getString( getConfigPath( id, path ) );
	}

	public Date getDatePath( Object id, String path )
	{
		return getDate( getConfigPath( id, path ) );
	}

	public List<?> getListPath( Object id, String path, Integer depth )
	{
		return getList( getConfigPath( id, path ), depth );
	}

	public Map<?, ?> getMapPath( Object id, String path, Integer depth )
	{
		return getMap( getConfigPath( id, path ), depth );
	}
	
	/////////////////
	// NODE VALUES //
	/////////////////

	public Boolean hasValue( Object id )
	{
		if (id == null)
			return false;
		return getConf( id ).hasValue();
	}

	public Object getValue( Object id )
	{
		if (id == null)
			return null;
		return getConf( id ).getValue();
	}

	public Boolean getBoolean( Object id )
	{
		if (id == null)
			return null;
		return getConf( id ).getBoolean();
	}

	public Integer getInteger( Object id )
	{
		if (id == null)
			return null;
		return getConf( id ).getInteger();
	}

	public Double getDouble( Object id )
	{
		if (id == null)
			return null;
		return getConf( id ).getDouble();
	}

	public String getString( Object id )
	{
		if (id == null)
			return null;
		return getConf( id ).getString();
	}

	public Date getDate( Object id )
	{
		if (id == null)
			return null;
		return getConf( id ).getDate();
	}

	public List<?> getList( Object id, Integer depth )
	{
		if (id == null)
			return null;
		return getConf( id ).getList( depth );
	}

	public Map<?, ?> getMap( Object id, Integer depth )
	{
		if (id == null)
			return null;
		return getConf( id ).getMap( depth );
	}
	
	/////////////////////////
	// CHANGE NOTIFICATION //
	/////////////////////////

	public void subscribe( Object id )
	{
		if (id == null)
			return;
		
		int iid = fromId( id );
		getConf0( iid );
		
		synchronized (subs)
		{
			if (subs.isEmpty())
				AlarmManager.staticAdd( LISTENER, null, INTERVAL );
			
			subs.add( iid );
		}
		
		fireConfigValuesChanged( new Object[] { id } );
	}
	
	private void fireConfigValuesChanged( final Object[] updated )
	{
		TodoManager.addTodo( new Todo()
		{

			public void doit( TodoManager mgr ) throws Exception
			{
				client.configValuesChanged( updated );
			}

			public void exception( TodoManager mgr, Exception e )
			{
				e.printStackTrace();
			}
		} );
	}
	
	public void subscribePath( Object id, String path )
	{
		subscribe( getConfigPath( id, path ) );
	}

	public void unsubscribe( Object id )
	{
		if (id == null)
			return;
		
		int iid = fromId( id );
		getConf0( iid );
		
		synchronized (subs)
		{
			if (!subs.remove( iid ))
				return;
			
			if (subs.isEmpty())
				AlarmManager.staticRemove( LISTENER );
		}
	}

	public void unsubscribePath( Object id, String path )
	{
		unsubscribe( getConfigPath( id, path ) );
	}
	
	public void unsubscribeAll()
	{
		if (subs == null)
			return;
		
		AlarmManager.staticRemove( LISTENER );
		
		synchronized (subs)
		{
			for (Integer iid: subs)
				getConf0( iid );
			subs.clear();
		}
	}

	/////////////
	// PRIVATE //
	/////////////
	
	///CLOVER:OFF
	
	/**
	 * Dumps the config space.
	 */
	public void dump()
	{
		for (Map.Entry<Integer, Conf> me: configs.entrySet())
			System.out.printf( "%d: %s\n", me.getKey(), me.getValue() );
		System.out.println( "done" );
	}
	
	///CLOVER:ON
	
	private static final int ID_RADIX = Character.MAX_RADIX;
	
	private String abs2rel( String path )
	{
		Assertion.check( isAbsolute( path ), "path is absolute" );
		return path.substring( 1 );
	}

	private static boolean isAbsolute( String path )
	{
		checkPath( path );
		return path.startsWith( "/" );
	}
	
	private static void checkPath( String path )
	{
		if (path == null)
			throw new IllegalArgumentException( "path == null" );
	}

	private Object toId( Integer iid )
	{
		if (iid == null)
			return null;
		return "#"+Integer.toString( iid, ID_RADIX )+"#";
	}

	private int fromId( Object id )
	{
		if (id == null)
			throw new IllegalArgumentException( "id == null" );
		
		if (!(id instanceof String))
			throw new IllegalArgumentException( "id is not valid: "+id );
		
		String s = (String) id;
		
		if (!s.startsWith( "#" ))
			throw new IllegalArgumentException( "id is not valid: "+id );
		
		if (!s.endsWith( "#" ))
			throw new IllegalArgumentException( "id is not valid: "+id );
		
		try
		{
			return Integer.parseInt( s.substring( 1 )
				.substring( 0, s.length()-2 ), ID_RADIX );
		}
		catch ( StringIndexOutOfBoundsException e )
		{
			throw new IllegalArgumentException( "id is not valid: "+id );
		}
		catch ( NumberFormatException e )
		{
			throw new IllegalArgumentException( "id is not valid: "+id );
		}
	}
	
	private int INTERVAL = 5*1000;
	
	private final static int FAIL_INTERVAL = 60*1000;

	private static final String configDir = ".";
	
	private final AlarmListener LISTENER = new AlarmListener()
	{
		public int wakeup( AlarmManager manager, Object state, long due )
		{
			try
			{
				updateConfig();
				return INTERVAL;
			}
			catch ( Exception e )
			{
				e.printStackTrace();
				return FAIL_INTERVAL;
			}
		}
	};
	
	private File mkfile( String name ) throws ConfigurationException
	{
		if (name == null || name.length() == 0)
			throw new ConfigurationException( "name is null or blank" );
		
		// filter out references to non-existent configs.
		
		File f = new File( name+".yml" );
		if (!f.isFile() || !f.canRead())
			throw new ConfigurationException( "could not verify name: "+name );
		
		// make sure the config is in the config directory.
		
		try
		{
			String fcp = f.getCanonicalPath();
//			System.out.println( "fcp = "+fcp );
			
			String cd = new File( configDir ).getCanonicalPath() + File.separatorChar;
//			System.out.println( "cd = "+cd );
			
			if (!fcp.startsWith( cd ))
				throw new ConfigurationException( "could not verify name: "+name );
		}
		catch ( IOException e )
		{
			throw new ConfigurationException( "could not verify name: "+name );
		}
		
		return f;
	}

	private File file;
	
	private Long lastModified;

	private Map<Integer, Conf> configs;
	
	private IdGenerator idgen = new IdGenerator( 0 );
	
	private Set<Integer> subs;
	
	private static Object loadConfig0( File file ) throws ConfigurationException
	{
		try
		{
			Object o = Yaml.load( file );
			return o;
		}
		catch ( FileNotFoundException e )
		{
			throw new ConfigurationException( "file not found: " + file );
		}
		catch ( YamlException e )
		{
			throw new ConfigurationException( "file not loaded: " + file + "; "+e );
		}
	}

	private Map<Integer, Conf> importConfigs( Object root )
	{
		Map<Integer, Conf> newConfigs = new HashMap<Integer, Conf>();
		importObject( newConfigs, null, null, root );
		return newConfigs;
	}
	
	/**
	 * This is purely for testing.
	 */
	void setConfig( String name ) throws Exception
	{
		file = mkfile( name );
		lastModified = Long.MIN_VALUE;
	}
	
	private void updateConfig() throws ConfigurationException
	{
		if (file.lastModified() == lastModified)
			return;
		
		long t0 = file.lastModified();
		Object o = loadConfig0( file );
		long t1 = file.lastModified();
		
		if (t1 != t0)
			return;
		
		Set<Object> changeIds = new HashSet<Object>();
		
		synchronized (subs)
		{
			Map<Integer, Conf> newConfigs = new HashMap<Integer, Conf>( configs );
			Set<Integer> newSubs = Collections.synchronizedSet(
				new HashSet<Integer>( subs ) );
			Set<Integer> changeSet = new HashSet<Integer>();
			
			updateObject( newConfigs, newSubs, changeSet, 0,
				null, null, o );
			
			// compute those ids in changeSet or their ancestors that
			// are subscribed to.
			computeChangeIds( changeIds, newConfigs, newSubs, changeSet );

			lastModified = t0;
			configs = newConfigs;
			subs = newSubs;
		}
		
		fireConfigValuesChanged( changeIds.toArray() );
	}

	private void computeChangeIds( Set<Object> changeIds,
		Map<Integer, Conf> newConfigs, Set<Integer> newSubs, Set<Integer> changeSet )
	{
		for (int iid: changeSet)
			computeChangeIds( changeIds, newConfigs, newSubs, iid );
	}

	private void computeChangeIds( Set<Object> changeIds,
		Map<Integer, Conf> newConfigs, Set<Integer> newSubs, int iid )
	{
		if (newSubs.contains( iid ))
		{
			changeIds.add( toId( iid ) );
		}
		else
		{
			Conf c = newConfigs.get( iid );
			if (c.parent != null)
				computeChangeIds( changeIds, newConfigs, newSubs, c.parent );
		}
	}

	private int importObject( Map<Integer, Conf> newConfigs, Integer parent, Object nameOrIndex,
		Object value )
	{
		if (valueIsList( value ))
			return importList( newConfigs, parent, nameOrIndex, (List<?>) value );
		
		if (valueIsMap( value ))
			return importMap( newConfigs, parent, nameOrIndex, (Map<?, ?>) value );
		
		if (valueIsScalar( value ))
		{
			int k = (int) idgen.next();
			newConfigs.put( k, new Conf( k, parent, nameOrIndex, value ) );
			return k;
		}
		
		throw new IllegalArgumentException(
			"don't know how to import type "+value.getClass() );
	}
	
	private static boolean valueIsScalar( Object value )
	{
		return value instanceof Boolean || value instanceof Number ||
			value instanceof String || value instanceof Date;
	}

	private boolean valueIsList( Object value )
	{
		return value instanceof List;
	}

	private boolean valueIsMap( Object value )
	{
		return value instanceof Map;
	}

	private void updateObject( Map<Integer, Conf> newConfigs, Set<Integer> newSubs,
		Set<Integer> changeSet, int iid, Integer parent, Object nameOrIndex,
		Object value )
	{
		Conf c = newConfigs.get( iid );
		
		Assertion.check( (parent == null && c.parent == null) ||
			(parent != null && c.parent != null && parent.equals( c.parent )),
				"parents match" );
		
		Assertion.check( (nameOrIndex == null && c.nameOrIndex == null) ||
			(nameOrIndex != null && c.nameOrIndex != null && nameOrIndex.equals( c.nameOrIndex )),
			"nameOrIndex match" );
		
		if (valueIsList( value ))
		{
			if (c.isList())
			{
				updateList( newConfigs, newSubs, changeSet, iid, parent, nameOrIndex, (List<?>) value, c );
				return;
			}
			if (c.isMap())
			{
				destroyMap( newConfigs, newSubs, iid, c );
				importList( newConfigs, newSubs, changeSet, iid, parent, nameOrIndex, (List<?>) value );
				return;
			}
			if (c.isScalar())
			{
				importList( newConfigs, newSubs, changeSet, iid, parent, nameOrIndex, (List<?>) value );
				return;
			}
			throw new IllegalArgumentException(
				"don't know how to update to list from type " + c.value.getClass() );
		}
		
		if (valueIsMap( value ))
		{
			if (c.isMap())
			{
				updateMap( newConfigs, newSubs, changeSet, iid, parent, nameOrIndex, (Map<?, ?>) value, c );
				return;
			}
			if (c.isList())
			{
				destroyList( newConfigs, newSubs, iid, c );
				importMap( newConfigs, newSubs, changeSet, iid, parent, nameOrIndex, (Map<?, ?>) value );
				return;
			}
			if (c.isScalar())
			{
				importMap( newConfigs, newSubs, changeSet, iid, parent, nameOrIndex, (Map<?, ?>) value );
				return;
			}
			throw new IllegalArgumentException(
				"don't know how to update to map from type " + c.value.getClass() );
		}
		
		if (valueIsScalar( value ))
		{
			if (c.isMap())
			{
				destroyMap( newConfigs, newSubs, iid, c );
				newConfigs.put( iid, new Conf( iid, parent, nameOrIndex, value ) );
				add( newConfigs, changeSet, iid );
				return;
			}
			
			if (c.isList())
			{
				destroyList( newConfigs, newSubs, iid, c );
				newConfigs.put( iid, new Conf( iid, parent, nameOrIndex, value ) );
				add( newConfigs, changeSet, iid );
				return;
			}
			
			if (c.isScalar())
			{
				if (value.equals( c.value ))
					return;
				
				newConfigs.put( iid, new Conf( iid, parent, nameOrIndex, value ) );
				add( newConfigs, changeSet, iid );
				return;
			}
		}
		
		throw new IllegalArgumentException( "don't know how to update to " +
			value.getClass() + " from " + c.value.getClass() );
	}

	private void add( Map<Integer, Conf> newConfigs, Set<Integer> changeSet, int iid )
	{
		System.out.println( "changeSet.add( "+iid+" )" );
		System.out.println( "  conf: "+newConfigs.get( iid ) );
//		new Throwable().printStackTrace( System.out );
		
		changeSet.add( iid );
	}

	private int importList( Map<Integer, Conf> newConfigs, Integer parent, Object nameOrIndex, List<?> value )
	{
		List<Integer> v = new ArrayList<Integer>( value.size() );
		int k = (int) idgen.next();
		newConfigs.put( k, new Conf( k, parent, nameOrIndex, Collections.unmodifiableList( v ) ) );
		
		int i = 0;
		for (Object o: value )
			v.add( importObject( newConfigs, k, i++, o ) );
		
		return k;
	}

	private void importList( Map<Integer, Conf> newConfigs, Set<Integer> newSubs,
		Set<Integer> changeSet, int iid, Integer parent, Object nameOrIndex,
		List<?> value )
	{
		List<Integer> v = new ArrayList<Integer>( value.size() );
		newConfigs.put( iid, new Conf( iid, parent, nameOrIndex, Collections.unmodifiableList( v ) ) );
		add( newConfigs, changeSet, iid );
		
		int i = 0;
		for (Object o: value )
			v.add( importObject( newConfigs, iid, i++, o ) );
	}

	private void updateList( Map<Integer, Conf> newConfigs, Set<Integer> newSubs,
		Set<Integer> changeSet, int iid, Integer parent, Object nameOrIndex,
		List<?> value, Conf c )
	{
		List<Integer> newList = new ArrayList<Integer>( c.list() );
		boolean changed = false;
		
		int n = Math.max( value.size(), newList.size() );
		for (int i = 0; i < n; i++)
		{
			if (i < value.size() && i < newList.size())
			{
				updateObject( newConfigs, newSubs, changeSet, newList.get( i ), iid, i, value.get( i ) );
			}
			else if (i < newList.size()) // ran out of new values (newList too long)
			{
				destroy( newConfigs, newSubs, newList.get( i ) );
				newList.set( i, null );
				changed = true;
				// this entry will eventually be removed...
			}
			else // i < value.size() // extending newList
			{
				newList.add( importObject( newConfigs, iid, i, value.get( i ) ) );
				changed = true;
			}
		}
		
		// trim newList if it is too long.
		
		while (newList.size() > value.size())
			newList.remove( newList.size() - 1 );
		
		newConfigs.put( iid, new Conf( iid, parent, nameOrIndex, Collections.unmodifiableList( newList ) ) );
		
		if (changed)
			add( newConfigs, changeSet, iid );
	}

	private void destroyList( Map<Integer, Conf> newConfigs, Set<Integer> newSubs, int iid, Conf c )
	{
		newConfigs.remove( iid );
		newSubs.remove( iid );
		for (int i: c.list())
			destroy( newConfigs, newSubs, i );
	}
	
	private int importMap( Map<Integer, Conf> newConfigs, Integer parent, Object nameOrIndex, Map<?, ?> value )
	{
		Map<String, Integer> v = new HashMap<String, Integer>( value.size() * 4 / 3 + 1 );
		int k = (int) idgen.next();
		newConfigs.put( k, new Conf( k, parent, nameOrIndex, Collections.unmodifiableMap( v ) ) );
		
		for (Map.Entry<?, ?> me: value.entrySet() )
		{
			String n = (String) me.getKey();
			v.put( n, importObject( newConfigs, k, n, me.getValue() ) );
		}
		
		return k;
	}

	private void importMap( Map<Integer, Conf> newConfigs, Set<Integer> newSubs,
		Set<Integer> changeSet, int iid, Integer parent, Object nameOrIndex,
		Map<?, ?> value )
	{
		Map<String, Integer> v = new HashMap<String, Integer>( value.size() * 4 / 3 + 1 );
		newConfigs.put( iid, new Conf( iid, parent, nameOrIndex, Collections.unmodifiableMap( v ) ) );
		add( newConfigs, changeSet, iid );
		
		for (Map.Entry<?, ?> me: value.entrySet() )
		{
			String n = (String) me.getKey();
			v.put( n, importObject( newConfigs, iid, n, me.getValue() ) );
		}
	}

	private void updateMap( Map<Integer, Conf> newConfigs, Set<Integer> newSubs,
		Set<Integer> changeSet, int iid, Integer parent, Object nameOrIndex,
		Map<?, ?> value, Conf c )
	{
		Map<String, Integer> newMap = new HashMap<String, Integer>( c.map() );
		boolean changed = false;
		
		// Look for names in the current map which are not in the new.
		
		for (Map.Entry<String, Integer> me: c.map().entrySet())
		{
			String name = me.getKey();
			Integer siid = me.getValue();
			if (!value.containsKey( name ))
			{
				// this name will have to be deleted.
				newMap.remove( name );
				destroy( newConfigs, newSubs, siid );
				changed = true;
			}
		}
		
		// Look for names in the new map which are not in the current.
		
		for (Map.Entry<?, ?> me: value.entrySet())
		{
			String name = (String) me.getKey();
			Object v = me.getValue();
			if (!newMap.containsKey( name ))
			{
				// import the object
				int k = importObject( newConfigs, iid, name, v );
				newMap.put( name, k );
				changed = true;
			}
			else
			{
				// update the object
				Integer siid = newMap.get( name );
				updateObject( newConfigs, newSubs, changeSet, siid, iid, name, v );
			}
		}
		
		newConfigs.put( iid, new Conf( iid, parent, nameOrIndex, Collections.unmodifiableMap( newMap ) ) );
		
		if (changed)
			add( newConfigs, changeSet, iid );
	}

	private void destroy( Map<Integer, Conf> newConfigs, Set<Integer> newSubs, int iid )
	{
		Conf c = newConfigs.get( iid );
		if (c.isMap())
			destroyMap( newConfigs, newSubs, iid, c );
		else if (c.isList())
			destroyList( newConfigs, newSubs, iid, c );
		else if (c.isScalar())
		{
			newConfigs.remove( iid );
			newSubs.remove( iid );
		}
		else
			throw new UnsupportedOperationException(
				"don't know how to destroy "+c.value.getClass() );
	}

	private void destroyMap( Map<Integer, Conf> newConfigs, Set<Integer> newSubs, int iid, Conf c )
	{
		newConfigs.remove( iid );
		newSubs.remove( iid );
		for (int i: c.map().values())
			destroy( newConfigs, newSubs, i );
	}
	
	private Conf getConf0( int iid )
	{
		if (!isLoaded())
			throw new IllegalStateException( "no config loaded" );
		
		Conf c = configs.get( iid );
		if (c == null)
			throw new IllegalArgumentException( "no such conf" );
		return c;
	}
	
	private Conf getConf( Object id )
	{
		return getConf0( fromId( id ) );
	}

	private Object[] toIdArray( Collection<Integer> value, Integer offset,
		Integer count )
	{
		int size = value.size();
		
		if (offset == null)
			offset = 0;
		else if (offset < 0 || offset > size)
			throw new IllegalArgumentException(
				"offset < 0 || offset > size" );
		
		if (count == null)
			count = Integer.MAX_VALUE;
		else if (count < 0)
			throw new IllegalArgumentException(
				"count < 0" );
		
		int remaining = size - offset;
		if (count > remaining)
			count = remaining;
		
		Object[] a = new Object[count];
		
		int i = 0;
		for (int v: value)
		{
			if (offset > 0)
			{
				offset--;
				continue;
			}
			
			if (i >= a.length)
				break;
			
			a[i++] = toId( v );
		}
		
		return a;
	}
	
	private Integer getConfigName( int iid, String name )
	{
		if (name.length() == 0)
			return iid;
		
		if (name.equals( "." ))
			return iid;
		
		Conf c = getConf0( iid );
		
		if (name.equals( ".." ))
			return c.parent;
		
		if (c.isList())
		{
			int i = Integer.valueOf( name );
			// i might be out of range.
			List<Integer> list = c.list();
			if (i < 0 || i >= list.size())
				throw new IllegalArgumentException( "i < 0 || i >= list.size()" );
			return list.get( i );
		}
		
		if (c.isMap())
		{
			Integer i = c.map().get( name );
			// i might be null.
			return i;
		}
		
		throw new NoSuchElementException( name );
	}
	
	private class Conf
	{
		public Conf( int id, Integer parent, Object nameOrIndex, Object value )
		{
			if (parent == null && nameOrIndex != null)
				throw new IllegalArgumentException(
					"parent == null && nameOrIndex != null" );
			
			if (parent != null && nameOrIndex == null)
				throw new IllegalArgumentException(
					"parent != null && nameOrIndex == null" );
			
			if (value == null)
				throw new IllegalArgumentException( "value == null" );
			
			this.id = id;
			this.parent = parent;
			this.nameOrIndex = nameOrIndex;
			this.value = value;
		}
		
		private final int id;

		public boolean isScalar()
		{
			return valueIsScalar( value );
		}

		public String getPath()
		{
			if (isRoot())
				return "/";
			
			String s = getConf0( parent ).getPath();
			
			if (s.equals( "/" ))
				return s + nameOrIndex;
			
			return s + '/' + nameOrIndex;
		}

		public Integer size()
		{
			if (isList())
				return list().size();
			
			if (isMap())
				return map().size();
			
			throw new NoSuchElementException( "value is not a List or a Map" );
		}

		public Integer index()
		{
			if (nameOrIndex == null)
				return null;
			
			if (nameOrIndex instanceof Integer)
				return (Integer) nameOrIndex;
			
			return null;
		}

		public String name()
		{
			if (nameOrIndex == null)
				return "";
			
			if (nameOrIndex instanceof String)
				return (String) nameOrIndex;
			
			return nameOrIndex.toString();
		}

		public final Integer parent;
		
		public final Object nameOrIndex;
		
		public final Object value;
		
		///CLOVER:OFF
		
		@Override
		public String toString()
		{
			return String.format( "Conf %d parent %s nameOrIndex %s, value %s", id, parent, nameOrIndex, value );
		}
		
		///CLOVER:ON

		public boolean isRoot()
		{
			return nameOrIndex == null;
		}
		
		public boolean isList()
		{
			return valueIsList( value );
		}

		public boolean isMap()
		{
			return valueIsMap( value );
		}
		
		@SuppressWarnings("unchecked")
		public List<Integer> list()
		{
			return (List<Integer>) value;
		}
		
		@SuppressWarnings("unchecked")
		public Map<String, Integer> map()
		{
			return (Map<String, Integer>) value;
		}
		
		public boolean hasValue()
		{
			return value != null;
		}
		
		public Object getValue()
		{
			return value;
		}
		
		public Boolean getBoolean()
		{
			if (value instanceof Boolean)
				return (Boolean) value;
			
			if (value instanceof String)
				return Boolean.valueOf( (String) value );
			
			throw new IllegalArgumentException( "cannot convert value to Boolean" );
		}
		
		public Integer getInteger()
		{
			if (value instanceof Integer)
				return (Integer) value;
			
			if (value instanceof String)
			{
				try
				{
					return Integer.valueOf( (String) value );
				}
				catch ( NumberFormatException e )
				{
					throw new IllegalArgumentException( "cannot convert value to Integer: "+value );
				}
			}
			
			throw new IllegalArgumentException( "cannot convert value to Integer" );
		}
		
		public Double getDouble()
		{
			if (value instanceof Double)
				return (Double) value;
			
			if (value instanceof Number)
				return ((Number) value).doubleValue();
			
			if (value instanceof String)
			{
				try
				{
					return Double.valueOf( (String) value );
				}
				catch ( NumberFormatException e )
				{
					throw new IllegalArgumentException( "cannot convert value to Double: "+value );
				}
			}
			
			throw new IllegalArgumentException( "cannot convert value to Double" );
		}
		
		public String getString()
		{
			if (value instanceof Boolean)
				return value.toString();
			if (value instanceof Number)
				return value.toString();
			if (value instanceof Date)
				return value.toString();
			if (value instanceof String)
				return (String) value;
			throw new IllegalArgumentException( "cannot convert value to String" );
		}
		
		@SuppressWarnings("deprecation")
		public Date getDate()
		{
			if (value instanceof Date)
				return (Date) value;
			if (value instanceof String)
				return new Date( (String) value );
			throw new IllegalArgumentException( "cannot convert value to Date" );
		}
		
		public List<?> getList( Integer depth )
		{
			if (isList())
				return getList0( depth );
			
			throw new IllegalArgumentException( "cannot convert value to List" );
		}
		
		private List<?> getList0( Integer depth )
		{
			if (depth == null)
				depth = Integer.MAX_VALUE;
			
			if (depth <= 0)
				return null;
			
			List<Object> l = new ArrayList<Object>( list().size() );
			for (Integer iid: list())
			{
				Conf c = getConf0( iid );
				if (c.isList())
					l.add( c.getList0( depth-1 ) );
				else if (c.isMap())
					l.add( c.getMap0( depth-1 ) );
				else
					l.add( c.getValue() );
			}
			return l;
		}

		public Map<?, ?> getMap( Integer depth )
		{
			if (isMap())
				return getMap0( depth );
			
			throw new IllegalArgumentException( "cannot convert value to Map: "+value.getClass() );
		}

		private Map<?, ?> getMap0( Integer depth )
		{
			if (depth == null)
				depth = Integer.MAX_VALUE;
			
			if (depth <= 0)
				return null;
			
			Map<String, Object> m = new HashMap<String, Object>( (map().size() * 4 + 2) / 3 );
			for (Map.Entry<String, Integer> e: map().entrySet())
			{
				Conf c = getConf0( e.getValue() );
				if (c.isList())
					m.put( e.getKey(), c.getList0( depth-1 ) );
				else if (c.isMap())
					m.put( e.getKey(), c.getMap0( depth-1 ) );
				else
					m.put( e.getKey(), c.getValue() );
			}
			return m;
		}
	}

	void setInterval( int newInterval )
	{
		INTERVAL = newInterval;
	}
}
