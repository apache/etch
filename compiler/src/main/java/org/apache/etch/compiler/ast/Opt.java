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

import java.util.HashSet;
import java.util.Set;

import org.apache.etch.compiler.ParseException;


/**
 * Abstract base class of annotations from the etch idl.
 */
abstract public class Opt
{
	/**
	 * Constructs the Opt.
	 *
	 * @param name the name of this opt. must be unique.
	 * @param lineno the line number of this opt.
	 */
	public Opt( String name, int lineno )
	{
		this.name = name;
		this.lineno = lineno;
	}
	
	private final String name;
	
	private final int lineno;
	
	private final Set<Class<?>> types = new HashSet<Class<?>>();
	
	/**
	 * @param type adds the specified type to the set of constructs
	 * to which this opt may be attached. generally only called from
	 * the constructor of an opt.
	 */
	public void addType( Class<?> type )
	{
		types.add( type );
	}

	/**
	 * @return the name of this option.
	 */
	public String name()
	{
		return name;
	}
	
	/**
	 * @return the line number of this opt.
	 */
	public int lineno()
	{
		return lineno;
	}

	/**
	 * Dumps the option nicely formatted.
	 * @param indent
	 */
	abstract public void dump( String indent );

	@Override
	abstract public String toString();

	/**
	 * @param type the type of construct to which the opt is attached.
	 * @throws ParseException
	 */
	public void check( Class<?> type ) throws ParseException
	{
		if (types != null)
		{
			if (!types.contains( type ))
				throw new ParseException( String.format(
					"option %s not allowed here, line %d", this, lineno) );
		}
	}
}
