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

/**
 * An option whose value is a string.
 */
public class StringArrayOption extends Option
{
	/**
	 * @param cp
	 * @param tokens
	 * @param names
	 * @param method
	 * @param description
	 * @param flags
	 * @param defaultValues
	 * @param constraints
	 * @throws Exception
	 */
	public StringArrayOption( CommandParser cp, String tokens, String[] names,
		String method, String description, int flags, String[] defaultValues,
		Constraint[] constraints ) throws Exception
	{
		super( cp, tokens, names, method, PARAMS, description, flags,
			defaultValues, constraints );
	}
	
	private final static Class<?>[] PARAMS =
		{ CommandParser.class, Option.class, String.class, Object[].class };

	@Override
	protected Object convertValue( String value )
	{
		return value;
	}
}
