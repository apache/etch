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
 * An enumeration item declaration from the etch idl.
 */
public class Item extends Named<Enumx>
{
	/**
	 * Constructs the Item.
	 *
	 * @param enumx
	 * @param name
	 */
	public Item( Enumx enumx, Name name )
	{
		super( enumx, name, null );
		
		vname = name.name;
	}
	
	private final String vname;
	
	@Override
	public String vname( Backend helper )
	{
		return helper.mfvname( vname );
	}
	
	@Override
	public String fqname()
	{
		return null;
	}
	
	@Override
	public String toString()
	{
		return name().toString();
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%sitem %s\n", indent, name().name );
		
		String sIndent = indent+"  ";
		
		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );
	}

	@Override
	public void check()
	{
		checkConstantName( name() );
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.doItem( this );
	}
}
