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

package org.apache.etch.compiler.ast;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.apache.etch.compiler.ParseException;


/**
 * Abstraction of a list of named items. The items may be retreived in
 * order added or by name.
 * @param <T>
 */
public class NameList<T> implements Iterable<T>
{
	/**
	 * Checks the name before it is added to the name list.
	 * @param name
	 * @throws ParseException
	 */
	public void check( Name name ) throws ParseException
	{
		if (map.containsKey( name.name ))
			throw new ParseException( String.format(
				"duplicate name (%s) at line %d", name.name, name.token.beginLine ) );
	}

	@Override
	public String toString()
	{
		return list.toString();
	}
	
	/**
	 * @param name
	 * @return the named item, or null.
	 */
	public T get( String name )
	{
		return map.get( name );
	}

	/**
	 * Adds the named item to the name list.
	 * @param name
	 * @param n
	 * @throws ParseException
	 */
	public void add( Name name, T n ) throws ParseException
	{
		check( name );
		map.put( name.name, n );
		list.add( n );
	}

	public Iterator<T> iterator()
	{
		return list.iterator();
	}

	private Map<String, T> map = new HashMap<String, T>();
	
	private List<T> list = new ArrayList<T>();

	/**
	 * @return the name list.
	 */
	public List<T> list()
	{
		return list;
	}

	/**
	 * @return true if the namelist is empty.
	 */
	public boolean isEmpty()
	{
		return list.isEmpty();
	}

	/**
	 * @param name
	 * @return true if the namelist contains the specified name.
	 */
	public boolean has( String name )
	{
		return map.containsKey( name );
	}
}
