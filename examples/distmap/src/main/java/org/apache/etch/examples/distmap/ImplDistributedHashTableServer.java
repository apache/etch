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

package org.apache.etch.examples.distmap;

import java.util.Map;


/**
 * Your custom implementation of BaseDistributedHashTableServer. Add methods here to provide
 * implementations of messages from the client.
 */
public class ImplDistributedHashTableServer extends BaseDistributedHashTableServer
{
	/**
	 * Constructs the ImplDistributedHashTableServer.
	 *
	 * @param client a connection to the client session. Use this to send a
	 * message to the client.
	 * @param map the global map.
	 */
	public ImplDistributedHashTableServer( RemoteDistributedHashTableClient client,
		Map<String, Object> map )
	{
		this.client = client;
		this.map = map;
	}
	
	@SuppressWarnings("unused")
	private final RemoteDistributedHashTableClient client;
	
	private final Map<String, Object> map;

	/**
	 * Gets all entries in the hash table.
	 * 
	 * @return an array of entries
	 */
	@Override
	public Entry[] getAll()
	{
		synchronized (map)
		{
			Entry[] list = new Entry[map.size()];
			int index = 0;
			for (Map.Entry<String, Object> me : map.entrySet())
				list[index++] = new Entry( me.getKey(), me.getValue() );
			return list;
		}
	}

	/**
	 * Gets the value to which key is mapped
	 * 
	 * @param key key associated with value
	 * @return the value to which the key is mapped
	 */
	@Override
	public Object getObject( String key )
	{
		return map.get( key );
	}

	/**
	 * Adds a key-value mapping to the map
	 * 
	 * @param key the key
	 * @param value the value
	 */
	@Override
	public Object putObject( String key, Object value )
	{
		return map.put( key, value );
	}

	/**
	 * Removes a key-value mapping from the map
	 * 
	 * @param key the key
	 * @return the value associated with key
	 */
	@Override
	public Object removeObject( String key )
	{
		return map.remove( key );
	}

	/**
	 * Get the number of key-value mappings in this map
	 * 
	 * @return the number of key-value mappings in this map
	 */
	@Override
	public Integer size()
	{
		return map.size();
	}
}