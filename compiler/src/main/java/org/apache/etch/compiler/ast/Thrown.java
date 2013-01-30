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
import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.ParseException;


/**
 * A thrown exception reference from the etch idl.
 */
public class Thrown extends Named<Message>
{
	/**
	 * Constructs the Thrown.
	 *
	 * @param mthd
	 * @param name
	 */
	public Thrown( Message mthd, Name name )
	{
		super( mthd, name, null );
	}

	@Override
	public String vname( Backend helper )
	{
		return null;
	}

	@Override
	public String fqname()
	{
		return null;
	}

	/**
	 * @return the named exception.
	 */
	public Named<?> getNamed()
	{
		if (name().token.kind != EtchGrammarConstants.ID && name().token.kind != EtchGrammarConstants.QID)
			return null;
		
		return parent().service().get( name().token.image );
	}
	
//	@Override
//	public Integer id()
//	{
//		return null;
//	}

	@Override
	public String toString()
	{
		return name().name;
	}
	@Override
	public void dump( String indent )
	{
		System.out.printf( "%sthrows %s\n", indent, name() );
		
		String sIndent = indent+"  ";
		
		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );
	}

	@Override
	public void check() throws ParseException
	{
		Named<?> n = getNamed();
		
		if (n == null)
			throw new ParseException( String.format(
				"undefined exception %s at line %d", name().token.image, name().token.beginLine ) );

		if (!(n instanceof Except))
			throw new ParseException( String.format(
				"%s not an exception at line %d", name().token.image, name().token.beginLine ) );
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.doThrown( this );
	}
}
