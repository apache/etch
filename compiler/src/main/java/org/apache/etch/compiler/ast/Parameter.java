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

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.ParseException;


/**
 * A parameter declaration from the etch idl. A parameter can be
 * a field of a struct or exception, or a parameter of a message.
 */
public class Parameter extends Named<ParamList<?>>
{
	/**
	 * Constructs the Parameter.
	 *
	 * @param pl
	 * @param rt
	 * @param name
	 */
	public Parameter( ParamList<?> pl, Name name, TypeRef rt )
	{
		super( pl, name, null );
		type = rt;
		vname = name.name;
	}
	
	private final TypeRef type;
	
	private final String vname;
	
	/**
	 * @return the type of this parameter.
	 */
	public TypeRef type()
	{
		return type;
	}
	
	@Override
	public String vname( Backend helper )
	{
		return helper.mfvname( vname );
	}

	@Override
	public String fqname()
	{
		return name().name;
	}
	
	@Override
	public String toString()
	{
		return type+" "+name().name;
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%sparam %s %s\n", indent, type, name().name );
		
		String sIndent = indent+"  ";
		
		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );
	}

	@Override
	public void check() throws ParseException
	{
		checkParamName( name() );
		
		type.check( parent().service() );
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.preParameter( this );
		
		type.treewalk( walker );
		
		walker.postParameter( this );
	}
}