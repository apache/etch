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

package org.apache.etch.bindings.java.msg;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;


/**
 * Map by id and name of IdNames (or subclasses thereof).
 * @param <T> the specific subclass of IdName that is in the map.
 */
abstract public class IdNameMap<T extends IdName>
{
	/**
	 * Constructs the IdNameMap.
	 */
	public IdNameMap()
	{
		// nothing to do.
	}
	
	/**
	 * Gets the IdName subclass which corresponds to the specified id.
	 * @param id the id of an IdName.
	 * @return the IdName subclass found.
	 */
	public final T get( Integer id )
	{
		return byId.get( id );
	}
	
	/**
	 * Gets the IdName subclass which corresponds to the specified
	 * name, or creates it if it isn't found and if this map is not
	 * locked.
	 * @param name the name of an IdName.
	 * @return the IdName subclass found or created.
	 * @see #lock()
	 */
	public final T get( String name )
	{
		T t = byName.get( name );
		if (t == null && !locked)
			t = add( makeNew( name ) );
		return t;
	}
	
	/**
	 * Adds the IdName subclass to the map.
	 * 
	 * @param t the IdName subclass to add.
	 * 
	 * @return the IdName from the map.
	 * 
	 * @throws IllegalStateException if locked.
	 * @throws IllegalArgumentException if there is a collision with an
	 * existing entry by id or name.
	 */
	public T add( T t )
	{
		if (locked)
			throw new IllegalStateException( "locked" );
		
		if (byId.containsKey( t.getId() ))
			throw new IllegalArgumentException( "id collision: "+t );

		if (byName.containsKey( t.getName() ))
			throw new IllegalArgumentException( "name collision: "+t );
		
		byId.put( t.getId(), t );
		byName.put( t.getName(), t );
		return t;
	}

	/**
	 * Adds all the IdNames to this IdNameMap.
	 * @param ts
	 * @throws IllegalStateException if locked.
	 * @throws IllegalArgumentException if there is a collision with an
	 * existing entry by id or name.
	 */
	public void addAll( IdNameMap<T> ts )
	{
		if (locked)
			throw new IllegalStateException( "locked" );
		
		for (T t: ts.byId.values())
			add( t );
	}

	/**
	 * @return a new set which is a snapshot of all the values.
	 */
	public final Set<T> values()
	{
		return new HashSet<T>( byId.values() );
	}

	/**
	 * Locks the map preventing further changes.
	 */
	public final void lock()
	{
		locked = true;
	}

	/**
	 * @return the number of values in the map.
	 */
	public int size()
	{
		return byId.size();
	}
	
	/**
	 * Makes a new subclass of IdName to put in this map.
	 * @param name the name of the new subclass of IdName.
	 * @return a newly constructed subclass of IdName to put in
	 * this map.
	 */
	abstract public T makeNew( String name );
	
	private final Map<Integer, T> byId = new HashMap<Integer, T>();
	
	private final Map<String, T> byName = new HashMap<String, T>();
	
	private boolean locked;
}
