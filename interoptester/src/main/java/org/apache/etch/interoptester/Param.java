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
 * Param models a named value initialized from args supplied to a function and
 * perhaps given a default value. A Param may be used when calling another
 * function by substitution into an Arg or Token.
 */
public class Param
{
	/**
	 * @param r
	 * @return the parsed Param.
	 */
	public static Param parse( TagElement r )
	{
		Assertion.check( r.matches( null, "param" ), "tag is param" );
		
		String name = r.getAttr( null, "name" );
		if (name == null || name.length() == 0)
			throw new IllegalArgumentException( "param tag has no name attribute" );
		
		Param p = new Param( name );
		p.parseBody( r );
		return p;
	}

	private Param( String name )
	{
		this.name = name;
	}

	private final String name;

	/**
	 * @return the name of this param.
	 */
	public String name()
	{
		return name;
	}

	private void parseBody( TagElement r )
	{
		deflt = r.getAttr( null, "default" );
		if (deflt == null && r.countChildren() > 0)
			deflt = r.getCdataValue();
	}
	
	private String deflt;

	/**
	 * @param args
	 * @return the value of the named param in args, or the default value if not
	 * in args.
	 */
	public String value( Map<String, String> args )
	{
		String v = args.get( name );
		if (v != null)
			return v;
		return deflt;
	}
	
	@Override
	public String toString()
	{
		return String.format( "Param( %s, %s )", name, deflt );
	}

	/**
	 * @param who the tag name of the owner of params.
	 * @param params a map of params (from param tags).
	 * @param overrides name / value pairs which override default param values.
	 * @return a map of name / value pairs which are the param values after
	 * overrides have been applied.
	 */
	public static Map<String, String> processOverrides( String who,
		Map<String, Param> params, Map<String, String> overrides )
	{
		// ensure names in overrides appear in params
		
		for (String name: overrides.keySet())
			if (!params.containsKey( name ))
				throw new IllegalArgumentException(
					who+" param '"+name+"' is not defined, cannot override" );
		
		// construct args out of params + overrides
		
		Map<String, String> args = new HashMap<String, String>();
		for (Param p : params.values())
		{
			String v = p.value( overrides );
			if (v == null)
				throw new IllegalArgumentException(
					who+" param '"+p.name()+"' has no value, must override" );
			args.put( p.name(), v );
		}
		return args;
	}
}