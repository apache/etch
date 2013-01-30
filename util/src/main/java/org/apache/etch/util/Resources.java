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

package org.apache.etch.util;

import java.util.HashMap;
import java.util.Map;

/**
 * A HashMap of string to object.
 */
public class Resources
{
	/**
	 * Constructs the Resources.
	 */
	public Resources()
	{
		this( null );
	}
	
	/**
	 * Constructs the Resources with a reference to related Resources.
	 * @param related
	 */
	public Resources( Resources related )
	{
		this.related = related;
	}
	
	private final Resources related;
	
	private final Map<String, Object> res = new HashMap<String, Object>();
	
	/**
	 * @return the related Resources.
	 */
	public Resources getRelated()
	{
		return related;
	}

	/**
	 * Returns true if this Resources or any related Resources has the key.
	 * @param key the string name of a resource.
	 * @return true if this Resources or any related Resources has the key.
	 */
	public boolean containsKey( String key )
	{
		return res.containsKey( key ) ||
			related != null && related.containsKey( key );
	}
	
	/**
	 * Returns the named resource from this Resources if present, otherwise
	 * searches for key in the related Resources.
	 * @param key the string name of a resource.
	 * @return the value of the resource.
	 */
	public Object get( String key )
	{
		Object o = res.get( key );
		
		if (o != null)
			return o;
		
		return related != null ? related.get( key ) : null;
	}
	
	/**
	 * Puts the named resource into this Resources. No related Resources are
	 * affected.
	 * @param key the string name of a resource.
	 * @param value the value of the resource.
	 * @return the old value of the key in this Resources only.
	 */
	public Object put( String key, Object value )
	{
		return res.put( key, value );
	}
	
	/**
	 * Removes the named resource from this Resources only. No related Resources
	 * are affected.
	 * @param key the string name of a resource.
	 * @return the old value of the key in this Resources only.
	 */
	public Object remove( String key )
	{
		return res.remove( key );
	}
}