/* $Id$
 *
 * Created by wert on Apr 5, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

import etch.bindings.java.msg.IdName;

/**
 * Map by id and name of IdNames (or subclasses thereof).
 * @param <T> the specific subclass of IdName that is in the map.
 */
abstract public class IdNameMap<T extends IdName>
{
	/**
	 * Constructs the IdNameMap.
	 * @param maxAutoCount the maximum number of automatically
	 * declared IdNames allowed. Set to 0 if you don't want any,
	 * but this may prevent working with a slightly different
	 * version of a service.
	 */
	public IdNameMap( int maxAutoCount )
	{
		this.maxAutoCount = maxAutoCount;
	}
	
	private final int maxAutoCount;
	
	/**
	 * Gets the IdName subclass which corresponds to the specified
	 * id, or creates it if it isn't found and if autoCount <
	 * maxAutoCount. If created, the IdName is given the name which
	 * is id.toString().
	 * @param id the id of an IdName.
	 * @return the IdName subclass found or created.
	 */
	public synchronized T get( Integer id )
	{
		T t = byId.get( id );
		if (t == null)
		{
			if (autoCount >= maxAutoCount)
				throw new IllegalArgumentException( "maxAutoCount would be exceeeded" );
			
			autoCount++;
			
			return add( makeNew( id, id.toString() ) );
		}
		return t;
	}
	
	/**
	 * Gets the IdName subclass which corresponds to the specified
	 * name, or creates it if it isn't found and if autoCount <
	 * maxAutoCount. If created, the IdName is given the id which
	 * is IdName.hash( name ).
	 * @param name the name of an IdName.
	 * @return the IdName subclass found or created.
	 */
	public synchronized T get( String name )
	{
		T t = byName.get( name );
		if (t == null)
		{
			if (autoCount >= maxAutoCount)
				throw new IllegalArgumentException( "maxAutoCount would be exceeeded" );
			
			autoCount++;
			
			return add( makeNew( IdName.hash( name ), name ) );
		}
		return t;
	}
	
	/**
	 * Adds the IdName subclass to the map. If the entry matches
	 * a current entry (both id and name), then return the current
	 * entry instead.
	 * 
	 * @param t the IdName subclass to add.
	 * 
	 * @return the IdName from the map, either the new one or a current
	 * one.
	 * 
	 * @throws IllegalArgumentException if there is a collision with
	 * id or name, or a collision with id and name where they are not
	 * associated with the same object.
	 */
	public synchronized T add( T t ) throws IllegalArgumentException
	{
		// four cases:
		// 1) t.id and t.name do not exist (no collision)
		// 2) t.id exists but t.name does not (collision)
		// 3) t.id does not exist but t.name does (collision)
		// 4) t.id and t.name exist (possible collision)

		T sameId = byId.get( t.getId() );
		T sameName = byName.get( t.getName() );
		if (sameId != null || sameName != null)
		{
			// sameId has the same id as t.
			// sameName has the same name as t.
			// the only way this isn't a problem is if sameId == sameName,
			// because that is the only case where there is a single
			// entry which has both the same id and name as t.
			
			if (sameId != sameName)
			{
				if (sameId != null && sameName != null)
					throw new IllegalArgumentException( String.format(
						"id and name collision: %d/%s vs. %d/%s and %d/%s",
						t.getId(), t.getName(), sameId.getId(), sameId.getName(), sameName.getId(), sameName.getName() ) );
				else if (sameId != null)
					throw new IllegalArgumentException( String.format(
						"id collision: %d/%s vs. %d/%s",
						t.getId(), t.getName(), sameId.getId(), sameId.getName() ) );
				else if (sameName != null)
					throw new IllegalArgumentException( String.format(
						"id collision: %d/%s vs. %d/%s",
						t.getId(), t.getName(), sameName.getId(), sameName.getName() ) );
			}
			
			return sameId;
		}
		
		byId.put( t.getId(), t );
		byName.put( t.getName(), t );
		return t;
	}

	/**
	 * @return a snapshot collection of all the values.
	 */
	public synchronized Collection<T> values()
	{
		return new ArrayList<T>( byId.values() );
	}
	
	/**
	 * Makes a new subclass of IdName to put in this map.
	 * @param id the id of the new subclass of IdName.
	 * @param name the name of the new subclass of IdName.
	 * @return a newly constructed subclass of IdName to put in
	 * this map.
	 */
	abstract public T makeNew( Integer id, String name );
	
	private final Map<Integer, T> byId = new HashMap<Integer, T>();
	
	private final Map<String, T> byName = new HashMap<String, T>();
	
	private int autoCount;
}
