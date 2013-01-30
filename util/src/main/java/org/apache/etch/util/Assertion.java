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

/**
 * Portable assertion checking.
 */
public class Assertion
{
	/**
	 * Checks a value for being true and if not it throws an exception
	 * with the specified description in the message.
	 * 
	 * @param value a boolean value which should be true (e.g., x < 100 ).
	 * 
	 * @param msg a description of value (e.g., "x < 100" ).
	 * 
	 * @throws AssertionException when value is not true.
	 */
	public static void check( boolean value, String msg )
		throws AssertionException
	{
		if (!value)
			throw new AssertionException( "assertion failed: "+msg );
	}

	/**
	 * Checks a value for being true and if not it throws an exception
	 * with the specified description in the message.
	 * 
	 * @param value a boolean value which should be true (e.g., x < 100 ).
	 * 
	 * @param fmt a description of value (e.g., "x (%d) < 100" ).
	 * 
	 * @param args args for the format.
	 * 
	 * @throws AssertionException when value is not true.
	 */
	public static void check( boolean value, String fmt, Object ... args )
		throws AssertionException
	{
		if (!value)
			throw new AssertionException( "assertion failed: "+
				String.format( fmt, args ) );
	}
}
