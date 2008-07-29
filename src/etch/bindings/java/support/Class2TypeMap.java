package etch.bindings.java.support;

import java.util.HashMap;
import java.util.Map;

import etch.bindings.java.msg.Type;

/**
 * Mapping between Class and Type.
 */
public class Class2TypeMap
{
	/**
	 * @param c
	 * @return the Type for the specified class, or null.
	 */
	public Type get( Class<?> c )
	{
		return c2t.get( c );
	}

	/**
	 * Adds a map entry from c to t.
	 * @param c
	 * @param t
	 * @throws IllegalStateException if locked.
	 * @throws IllegalArgumentException if conflict.
	 */
	public void put( Class<?> c, Type t )
	{
		if (locked)
			throw new IllegalStateException( "locked" );
		
		Type x = c2t.get( c );
		if (x != null)
		{
			if (!x.equals( t ))
				throw new IllegalArgumentException( String.format(
					"type %s: class %s is already mapped to type %s",
					t, c, x ) );
			
			// x.equals( t ) therefore our work is finished.
			return;
		}
		
		c2t.put( c, t );
	}

	/**
	 * Adds all the mappings from other to this.
	 * @param other
	 * @throws IllegalStateException if locked.
	 * @throws IllegalArgumentException if conflict.
	 */
	public void addAll( Class2TypeMap other )
	{
		if (locked)
			throw new IllegalStateException( "locked" );
		
		for (Map.Entry<Class<?>, Type> me: other.c2t.entrySet())
			put( me.getKey(), me.getValue() );
	}

	/**
	 * Locks the map, disallowing any more changes.
	 */
	public void lock()
	{
		locked = true;
	}
	
	private final Map<Class<?>, Type> c2t = new HashMap<Class<?>, Type>();
	
	private boolean locked;
}