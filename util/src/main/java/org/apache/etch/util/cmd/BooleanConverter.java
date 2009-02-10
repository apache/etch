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

package org.apache.etch.util.cmd;

import java.util.HashMap;
import java.util.Map;

/**
 * Handle translation of various types of boolean values.
 */
public class BooleanConverter
{
	/**
	 * @param value
	 * @return result of translation of boolean value.
	 */
	public static Boolean valueOf( String value )
	{
		Boolean v = values.get( value.toLowerCase() );
		if (v == null)
			throw new IllegalArgumentException( "bad boolean value: "+value );
		return v;
	}
	
	/**
	 * Defines static key pairs to use for true/false item conversions.
	 * @param trueKey
	 * @param falseKey
	 */
	public static void define( String trueKey, String falseKey )
	{
		if (trueKey != null)
			values.put( trueKey.toLowerCase(), Boolean.TRUE );
		
		if (falseKey != null)
			values.put( falseKey.toLowerCase(), Boolean.FALSE );
	}
	
	private final static Map<String,Boolean> values = new HashMap<String,Boolean>();
	static
	{
		define( "true", "false" );
		define( "yes", "no" );
		define( "1", "0" );
		define( "enable", "disable" );
	}
}
