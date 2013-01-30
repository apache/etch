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
 * Token models an unnamed positional parameter to a program.
 */
public class Token
{
	/**
	 * @param r
	 * @return the parsed Token.
	 */
	public static Token parse( TagElement r )
	{
		Assertion.check( r.matches( null, "token" ), "tag is token" );
		
		String value = r.getAttr( null, "value" );
		if (value == null && r.hasChildren())
			value = r.getCdataValue();
		
		return new Token( value );
	}
	
	private Token( String value )
	{
		this.value = value;
	}
	
	private final String value;
	
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
}
