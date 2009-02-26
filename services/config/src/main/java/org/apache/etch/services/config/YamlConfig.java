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


/** Local Yaml implementation of ConfigurationServer */
public class YamlConfig implements ConfigurationServer
{
	/**
	 * Constructs an empty YamlConfigurationServer.
	 * @param client 
	 */
	public YamlConfig( ConfigurationClient client )
	{
		this.client = client;
	}
	
	@SuppressWarnings("unused")
	private final ConfigurationClient client;
	
	/**
	 * Constructs a YamlConfigurationServer, then loads the config.
	 * @param client 
	 * @param name
	 * @throws ConfigurationException
	 */
	public YamlConfig( ConfigurationClient client, String name )
		throws ConfigurationException
	{
		this( client );
		loadConfig( name );
	}

	public void loadConfig( String name )
		throws ConfigurationException
	{
		unloadConfig();
		file = new File( name+".yml" );
		importConfigs( loadConfig() );
	}
	
	public void unloadConfig()
	{
		file = null;
		lastModified = 0;
		configs = new ArrayList<Conf>();
		unsubscribeAll();
	}
	
	private Object loadConfig() throws ConfigurationException
	{
		for (int i = 0; i < 4; i++)
		{
			try
			{
				long t0 = file.lastModified();
				Object o = Yaml.load( file );
				long t1 = file.lastModified();
				if (t1 != t0)
				{
					Thread.sleep( 1000 );
					continue;
				}
				lastModified = t1;
				return o;
			}
			catch ( FileNotFoundException e )
			{
				throw new ConfigurationException( "file not found: " + file );
			}
			catch ( InterruptedException e )
			{
				break;
			}
		}
		throw new ConfigurationException( "file changed while loading: " + file );
	}

	private File file;
	
	private long lastModified;

	public Boolean canLoad( String name )
	{
		return true;
	}

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

	public String getPath( Object id )
	{
		Conf c = getConf( id );
		
		if (c.isRoot())
			return "";
		
		return getPath( toId( c.parent ) ) + '/' + c.name;
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
		if (path == null || path.length() == 0)
		{
			getConf( id );
			return id;
		}
		
		// path is not null and not empty.
		
		Integer iid = fromId( id );
		
		if (path.indexOf( '/' ) < 0)
			return toId( getConfig0( iid, path ) );
		
		if (iid != null && path.startsWith( "/" ))
			iid = null;
		
		getConf( iid );
		
		StringTokenizer st = new StringTokenizer( path, "/" );
		while (st.hasMoreTokens())
			iid = getConfig0( iid, st.nextToken() );
		
		return toId( iid );
	}

	//////////////////////
	// NODE PATH VALUES //
	//////////////////////

	public Boolean hasValuePath( Object id, String path )
	{
		return hasValue( getConfigPath( id, path ) );
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
		return id != null && getConf( id ).hasValue();
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

	private Object toId( Integer iid )
	{
		return iid;
	}

	private Integer fromId( Object id )
	{
		if (id == null)
			return null;
		if (id instanceof Integer)
			return (Integer) id;
		return ((Number) id).intValue();
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
	
	private final Set<Integer> subs = Collections.synchronizedSet( new HashSet<Integer>() );

	/////////////
	// PRIVATE //
	/////////////

	private void importConfigs( Object root )
	{
		importObject( null, null, root );
	}
	
	private void updateConfig() throws ConfigurationException
	{
		if (file.lastModified() != lastModified)
		{
			List<Integer> changeSet = new ArrayList<Integer>();
			updateObject( changeSet, 0, null, null, loadConfig() );
			// TODO notify about changeSet
		}
	}

	private int importObject( Integer parent, Object name, Object value )
	{
		if (value instanceof List)
		{
			return importList( parent, name, (List<?>) value );
		}
		else if (value instanceof Map)
		{
			return importMap( parent, name, (Map<?, ?>) value );
		}
		else if (isScalar( value ))
		{
			int k = configs.size();
			configs.add( new Conf( parent, name, value ) );
			return k;
		}
		else
		{
			throw new IllegalArgumentException(
				"don't know how to import type "+value.getClass() );
		}
	}
	
	private boolean isScalar( Object value )
	{
		return value instanceof Boolean || value instanceof Number ||
			value instanceof String || value instanceof Date;
	}

	private void updateObject( List<Integer> changeSet, int id,
		Integer parent, Object name, Object value )
	{
		if (true) return;
		
		Conf c = getConf( id );
		
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

	private int importList( Integer parent, Object name, List<?> value )
	{
		List<Integer> v = new ArrayList<Integer>( value.size() );
		int k = configs.size();
		configs.add( new Conf( parent, name, v ) );
		
		int i = 0;
		for (Object o: value )
			v.add( importObject( k, i++, o ) );
		
		return k;
	}
	
	private int importMap( Integer parent, Object name, Map<?, ?> value )
	{
		Map<String, Integer> v = new HashMap<String, Integer>( value.size() * 4 / 3 + 1 );
		int k = configs.size();
		configs.add( new Conf( parent, name, v ) );
		
		for (Map.Entry<?, ?> me: value.entrySet() )
		{
			String n = (String) me.getKey();
			v.put( n, importObject( k, n, me.getValue() ) );
		}
		
		return k;
	}

	private List<Conf> configs;
	
	private Conf getConf( Integer iid )
	{
		if (iid == null)
			iid = 0;
//			throw new IllegalArgumentException( "id == null" );
		
		if (iid < 0 || iid >= configs.size())
			throw new IllegalArgumentException( "id < 0 || id >= configs.size()" );
		
		return configs.get( iid );
	}
	
	private Conf getConf( Object id )
	{
		return getConf( fromId( id ) );
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
				offset--;
			else
				a[i++] = toId( v );
		}
		
		return a;
	}
	
	private Integer getConfig0( Integer iid, String name )
	{
		Conf c = getConf( iid );
		
		if (name.equals( "." ))
			return iid;
		
		if (name.equals( ".." ))
			return c.parent;
		
		if (c.isList())
			return c.list().get( Integer.valueOf( name ) );
		
		if (c.isMap())
		{
			Integer i = c.map().get( name );
			// i might be null.
			return i;
		}
		
		throw new NoSuchElementException( name );
	}
	
	private static class Conf
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
		
		public Boolean getBoolean()
		{
			if (value == null)
				return null;
			if (value instanceof Boolean)
				return (Boolean) value;
			if (value instanceof String)
				return Boolean.valueOf( (String) value );
			throw new IllegalArgumentException( "cannot convert value to Boolean" );
		}
		
		public Integer getInteger()
		{
			if (value == null)
				return null;
			if (value instanceof Integer)
				return (Integer) value;
			if (value instanceof Number)
				return ((Number) value).intValue();
			if (value instanceof String)
				return Integer.valueOf( (String) value );
			throw new IllegalArgumentException( "cannot convert value to Integer" );
		}
		
		public Double getDouble()
		{
			if (value == null)
				return null;
			if (value instanceof Double)
				return (Double) value;
			if (value instanceof Number)
				return ((Number) value).doubleValue();
			if (value instanceof String)
				return Double.valueOf( (String) value );
			throw new IllegalArgumentException( "cannot convert value to Double" );
		}
		
		public String getString()
		{
			if (value == null)
				return null;
			if (value instanceof String)
				return (String) value;
			if (value instanceof Number)
				return value.toString();
			if (value instanceof Boolean)
				return value.toString();
			if (value instanceof Date)
				return value.toString();
			throw new IllegalArgumentException( "cannot convert value to String" );
		}
		
		@SuppressWarnings("deprecation")
		public Date getDate()
		{
			if (value == null)
				return null;
			if (value instanceof Date)
				return (Date) value;
			if (value instanceof String)
				return new Date( (String) value );
			throw new IllegalArgumentException( "cannot convert value to Date" );
		}
		
		public List<?> getList( Integer depth )
		{
			// TODO Auto-generated method stub
			return null;
		}
		
		public Map<?, ?> getMap( Integer depth )
		{
			// TODO Auto-generated method stub
			return null;
		}
	}

	public Integer getIndex( Object id )
	{
		// TODO Auto-generated method stub
		return null;
	}

	public Object getValue( Object id )
	{
		// TODO Auto-generated method stub
		return null;
	}

	public Object getValuePath( Object id, String path )
	{
		// TODO Auto-generated method stub
		return null;
	}

	public Boolean isLoaded()
	{
		// TODO Auto-generated method stub
		return null;
	}
}