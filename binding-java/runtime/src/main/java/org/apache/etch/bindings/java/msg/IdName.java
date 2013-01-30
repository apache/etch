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

import org.apache.etch.util.Hash;

/**
 * An IdName is a base class for Field or Type. It is used
 * to bind together a type or field name with the associated id. The id
 * is used for certain operations, such as the key in a Map, comparisons,
 * and binary encoding on the wire, while the name is used for display.
 * 
 * @see Field
 * @see Type
 */
public class IdName extends Hash
{
	/**
	 * Constructs the IdName.
	 * @param id the id for the name (normally computed using
	 * {@link #hash(String)})
	 * @param name the name of the item.
	 */
	public IdName( Integer id, String name )
	{
		this.id = id;
		this.name = name;
	}
	
	/**
	 * Constructs the IdName by computing the appropriate id given
	 * the name.
	 * @param name the name of the item.
	 * @see #hash(String)
	 */
	public IdName( String name )
	{
		this( hash( name ), name );
	}
	
	private final Integer id;
	
	private final String name;
	
	/**
	 * @return the id of the item.
	 */
	public final Integer getId()
	{
		return id;
	}
	
	/**
	 * @return the name of the item.
	 */
	public final String getName()
	{
		return name;
	}
	
	@Override
	public final String toString()
	{
		return String.format( "%s(%d)", name, id );
	}

	@Override
	public final boolean equals( Object obj )
	{
		if (obj == this)
			return true;
		
		if (obj == null)
			return false;
		
		if (getClass() != obj.getClass())
			return false;
		
		IdName other = (IdName) obj;
		
		return id.equals( other.id ) && name.equals( other.name );
	}

	@Override
	public final int hashCode()
	{
		return id.hashCode() ^ name.hashCode();
	}
}
