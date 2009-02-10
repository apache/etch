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

package org.apache.etch.interoptester;

import java.util.HashMap;
import java.util.Map;

import org.apache.etch.util.Assertion;
import org.apache.etch.util.core.xml.XmlParser.TagElement;


/**
 * Arg models an named value passed to a procedure (which is used to initialize
 * a Param once received).
 */
public class Arg
{
	/**
	 * @param r
	 * @return the Arg that was parsed.
	 */
	public static Arg parse( TagElement r )
	{
		Assertion.check( r.matches( null, "arg" ), "tag is arg" );
		
		String name = r.getAttr( null, "name" );
		if (name == null || name.length() == 0)
			throw new IllegalArgumentException( "arg tag has no name" );
		
		String value = r.getAttr( null, "value" );
		if (value == null && r.countChildren() > 0)
			value = r.getCdataValue();
		
		if (value == null)
			throw new IllegalArgumentException( "arg '"+name+"' has no value" );
		
		return new Arg( name, value );
	}

	private Arg( String name, String value )
	{
		this.name = name;
		this.value = value;
	}
	
	private final String name;
	
	private final String value;

	/**
	 * @return the name of this arg.
	 */
	public String name()
	{
		return name;
	}
	
	/**
	 * @param args named values which might be used as substitutions.
	 * @return the value of this arg. This might involve substituting named
	 * values from args.
	 */
	public String value( Map<String, String> args )
	{
		if (value == null)
			return null;
		
		return Substitutor.subst( value, args );
	}
	
	@Override
	public String toString()
	{
		return String.format( "Arg( %s, %s )", name, value );
	}

	/**
	 * @param who
	 * @param args
	 * @param substs
	 * @return the map formed by processing substitutions from substs into each
	 * value from args.
	 */
	public static Map<String, String> processSubst( String who,
		Map<String, Arg> args, Map<String, String> substs )
	{
		Map<String, String> nargs = new HashMap<String, String>();
		for (Arg a: args.values())
		{
			String v = a.value( substs );
			nargs.put( a.name(), v );
		}
		return nargs;
	}
}