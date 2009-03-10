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
	
	@SuppressWarnings("unused")
	private final ConfigurationClient client;

	////////////////////
	// PUBLIC METHODS //
	////////////////////

	public Object loadConfig( String name )
		throws ConfigurationException
	{
		unloadConfig();
		
		File f = mkfile( name );
		if (f == null)
			throw new ConfigurationException( "bad name "+name );
		
		for (int i = 0; i < 4; i++)
		{
			long t0 = f.lastModified();
			Object o = loadConfig0( f );
			long t1 = f.lastModified();
			
			if (t1 == t0)
			{
				file = f;
				lastModified = t0;
				configs = importConfigs( o );
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
		file = null;
		lastModified = 0;
		configs = null;
		unsubscribeAll();
	}

	public Boolean canLoad( String name )
	{
		File f = mkfile( name );
		if (f == null)
			return false;
		return f.isFile() && f.canRead();
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
		Object n = getConf( id ).name;
		if (n == null)
			return "";
		if (n instanceof String)
			return (String) n;
		return n.toString();
	}

	public Integer getIndex( Object id )
	{
		Object i = getConf( id ).name;
		if (i == null)
			return null;
		if (i instanceof Integer)
			return (Integer) i;
		return null;
	}

	public String getPath( Object id )
	{
		Conf c = getConf( id );
		
		if (c.isRoot())
			return "/";
		
		String s = getPath( toId( c.parent ) );
		
		if (s.equals( "/" ))
			return s + c.name;
		
		return s + '/' + c.name;
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
		Conf c = getConf( id );
		
		if (c.isList())
			return c.list().size();
		
		if (c.isMap())
			return c.map().size();
		
		throw new NoSuchElementException( "value is not a List or a Map" );
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
	
	public Object[] listConfigPathIds( Object id, String path, Integer offset, Integer count )
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
				throw new IllegalArgumentException( "index < 0 || index >= list.size()" );
			return toId( c.list().get( index ) );
		}
		
		throw new IllegalArgumentException( "node is not a list" );
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
				throw new IllegalArgumentException( "path must be absolute when id == null" );
			
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
			return toId( getConfig0( iid, path ) );
		
		// complex path
		
		StringTokenizer st = new StringTokenizer( path, "/" );
		while (iid != null && st.hasMoreTokens())
			iid = getConfig0( iid, st.nextToken() );
		
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
		
		getConf( id );
		
		synchronized (subs)
		{
			boolean wasEmpty = subs.isEmpty();
			
			subs.add( fromId( id ) );
			
			if (wasEmpty)
				AlarmManager.staticAdd( LISTENER, null, INTERVAL );
		}
	}

	public void subscribePath( Object id, String path )
	{
		subscribe( getConfigPath( id, path ) );
	}

	public void unsubscribe( Object id )
	{
		if (id == null)
			return;
		
		getConf( id );
		
		synchronized (subs)
		{
			subs.remove( id );
			
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
		synchronized (subs)
		{
			subs.clear();

			AlarmManager.staticRemove( LISTENER );
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
		int i = 0;
		for (Conf c : configs)
			System.out.printf( "%d: %s\n", i++, c );
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
	
	private final Set<Integer> subs = Collections.synchronizedSet( new HashSet<Integer>() );

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
			return Integer.parseInt( s.substring( 1 ).substring( 0, s.length()-2 ), ID_RADIX );
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
	
	private final static int INTERVAL = 5*1000;
	
	private final AlarmListener LISTENER = new AlarmListener()
	{
		public int wakeup( AlarmManager manager, Object state,
			long due )
		{
			try
			{
				updateConfig();
			}
			catch ( Exception e )
			{
				e.printStackTrace();
				return 0;
			}
			return INTERVAL;
		}
	};
	
	private File mkfile( String name )
	{
		if (name == null || name.length() == 0)
			return null;
		
		return new File( name+".yml" );
	}

	private File file;
	
	private long lastModified;

	private List<Conf> configs;
	
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

	private List<Conf> importConfigs( Object root )
	{
		List<Conf> c = new ArrayList<Conf>();
		importObject( c, null, null, root );
		return c;
	}
	
	private void updateConfig() throws ConfigurationException
	{
		if (file.lastModified() != lastModified)
		{
			List<Integer> changeSet = new ArrayList<Integer>();
			updateObject( changeSet, 0, null, null, loadConfig0( null ) );
			// TODO notify about changeSet
		}
	}

	private int importObject( List<Conf> c, Integer parent, Object name, Object value )
	{
		if (value instanceof List)
		{
			return importList( c, parent, name, (List<?>) value );
		}
		else if (value instanceof Map)
		{
			return importMap( c, parent, name, (Map<?, ?>) value );
		}
		else if (isScalar( value ))
		{
			int k = c.size();
			c.add( new Conf( parent, name, value ) );
			return k;
		}
		else
		{
			throw new IllegalArgumentException(
				"don't know how to import type "+value.getClass() );
		}
	}
	
	private static boolean isScalar( Object value )
	{
		return value instanceof Boolean || value instanceof Number ||
			value instanceof String || value instanceof Date;
	}

	private void updateObject( List<Integer> changeSet, int id,
		Integer parent, Object name, Object value )
	{
		if (true) return;
		
		Conf c = getConf0( id );
		
		Assertion.check( (parent == null && c.parent == null) ||
			(parent != null && c.parent != null && parent.equals( c.parent )),
				"parents match" );
		
		Assertion.check( (name == null && c.name == null) ||
			(name != null && c.name != null && name.equals( c.name )),
			"names match" );
		
		if (value instanceof List)
		{
			// return updateList( changeSet, parent, name, (List<?>) value, c.value );
			return;
		}
		else if (value instanceof Map)
		{
			// return updateMap( changeSet, parent, name, (Map<?, ?>) value );
		}
		else if (isScalar( value ))
		{
			if ((value == null && c.value == null) ||
				(value != null && c.value != null && value.equals( c.value )))
				return;
		}
		else
		{
			throw new IllegalArgumentException(
				"don't know how to update type "+value.getClass() );
		}
	}

	private int importList( List<Conf> c, Integer parent, Object name, List<?> value )
	{
		List<Integer> v = new ArrayList<Integer>( value.size() );
		int k = c.size();
		c.add( new Conf( parent, name, v ) );
		
		int i = 0;
		for (Object o: value )
			v.add( importObject( c, k, i++, o ) );
		
		return k;
	}
	
	private int importMap( List<Conf> c, Integer parent, Object name, Map<?, ?> value )
	{
		Map<String, Integer> v = new HashMap<String, Integer>( value.size() * 4 / 3 + 1 );
		int k = c.size();
		c.add( new Conf( parent, name, v ) );
		
		for (Map.Entry<?, ?> me: value.entrySet() )
		{
			String n = (String) me.getKey();
			v.put( n, importObject( c, k, n, me.getValue() ) );
		}
		
		return k;
	}
	
	private Conf getConf0( int iid )
	{
		if (!isLoaded())
			throw new IllegalStateException( "no config loaded" );
		
		if (iid < 0 || iid >= configs.size())
			throw new IllegalArgumentException( "id < 0 || id >= configs.size(): id = "+iid+", configs.size() = "+configs.size() );
		
		return configs.get( iid );
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
	
	private Integer getConfig0( int iid, String name )
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
		public Conf( Integer parent, Object name, Object value )
		{
			this.parent = parent;
			this.name = name;
			this.value = value;
		}

		public Integer parent;
		
		public Object name;
		
		public Object value;
		
		///CLOVER:OFF
		
		@Override
		public String toString()
		{
			return String.format( "Conf name %s, value %s", name, value );
		}
		
		///CLOVER:ON

		public boolean isRoot()
		{
			return name == null;
		}
		
		public boolean isList()
		{
			return value instanceof List;
		}

		public boolean isMap()
		{
			return value instanceof Map;
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
			
			throw new IllegalArgumentException( "cannot convert value to Map" );
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
}
