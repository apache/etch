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

package org.apache.etch.bindings.java.transport;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.apache.etch.bindings.java.msg.ValueFactory;


/**
 * A factory used to construct tagged data processors for Messagizer.
 */
abstract public class FormatFactory
{
	/**
	 * Standard name for etch binary tagged data format.
	 */
	public static final String BINARY = "binary";
	
	/**
	 * Standard name for etch xml tagged data format.
	 */
	public static final String XML = "xml";
	
	/**
	 * @param vf
	 * @param uri
	 * @return the tagged data input with the specified value factory and
	 * initialized by terms from the uri.
	 */
	abstract public TaggedDataInput newTaggedDataInput( ValueFactory vf,
		String uri );

	/**
	 * @param vf
	 * @param uri
	 * @return the tagged data output with the specified value factory and
	 * initialized by terms from the uri.
	 */
	abstract public TaggedDataOutput newTaggedDataOutput( ValueFactory vf,
		String uri );
	
	private final static Map<String, Object> formatFactories =
		Collections.synchronizedMap( new HashMap<String, Object>() );
	
	/**
	 * Gets the named format factory.
	 * @param name the name of a configured format factory.
	 * @return the named format factory, or null if it isn't defined.
	 */
	public static FormatFactory get( String name )
	{
		Object factory = formatFactories.get( name );
		
		if (factory == null)
			return null;
		
		if (factory instanceof String)
		{
			try
			{
				Class<?> c = Class.forName( (String) factory );
				factory = c.newInstance();
				if (!(factory instanceof FormatFactory))
					throw new IllegalArgumentException(
						"instance for format name "+name+" ("+
						factory+") does not implement FormatFactory" );
				formatFactories.put( name, factory );
				return (FormatFactory) factory;
			}
			catch ( Exception e )
			{
				throw new IllegalArgumentException(
					"could not create instance of format name "+name, e );
			}
		}
		
		return (FormatFactory) factory;
	}
	
	/**
	 * Puts a named format factory.
	 * @param name the name of this format factory (e.g., "binary").
	 * @param factory the fully qualified class name or instance
	 * of the FormatFactory to associate with the name.
	 */
	public static void put( String name, Object factory )
	{
		formatFactories.put( name, factory );
	}
	
	static
	{
		put( BINARY,
			"org.apache.etch.bindings.java.transport.fmt.binary.BinaryTaggedDataFormatFactory" );
	}
}
