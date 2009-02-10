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

import java.util.Map;

import org.apache.etch.util.Assertion;
import org.apache.etch.util.core.xml.XmlParser.TagElement;


/**
 * Env models a contribution to the environment of a running program.
 */
public class Env
{
	/**
	 * @param r
	 * @return the Env that was parsed.
	 */
	public static Env parse( TagElement r )
	{
		Assertion.check( r.matches( null, "env" ), "tag is env" );
		
		String name = r.getAttr( null, "name" );
		if (name == null || name.length() == 0)
			throw new IllegalArgumentException( "env tag has no name" );
		
		String value = r.getAttr( null, "value" );
		if (value == null && r.hasChildren())
			value = r.getCdataValue();
		
		return new Env( name, value );
	}
	
	private Env( String name, String value )
	{
		this.name = name;
		this.value = value;
	}
	
	private final String name;

	/**
	 * @return the name of this environment variable.
	 */
	public String name()
	{
		return name;
	}
	
	private final String value;
	
	/**
	 * @param args named values which might be used as substitutions.
	 * @return the value of this env. This might involve substituting named
	 * values from args.
	 */
	public String value( Map<String, String> args )
	{
		if (value == null)
			return null;
		
		return Substitutor.subst( value, args );
	}
}
