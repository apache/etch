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

import org.apache.etch.compiler.Token;

/**
 * A name of a thing from the etch idle. If the token is present,
 * it can be used to identify the line number of the name.
 */
public class Name
{
	/**
	 * Constructs the Name.
	 *
	 * @param token
	 * @param name
	 */
	public Name( Token token, String name )
	{
		this.token = token;
		this.name = name;
	}
	
	/**
	 * The original source of the name. Used to get the line number.
	 */
	public final Token token;
	
	/**
	 * The name.
	 */
	public final String name;
	
	/**
	 * @return the string of this name.
	 */
	public String name()
	{
		return name;
	}

	@Override
	public String toString()
	{
		return name;
	}
}