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

package org.apache.etch.compiler.opt;

import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.Token;
import org.apache.etch.compiler.ast.Except;
import org.apache.etch.compiler.ast.Name;
import org.apache.etch.compiler.ast.Opt;


/**
 * Option which sets the unchecked attribute of an exception.
 */
public class Unchecked extends Opt
{
	/**
	 * Constructs the Unchecked.
	 *
	 * @param name
	 * @param args
	 * @throws ParseException
	 */
	public Unchecked( Name name, Token... args ) throws ParseException
	{
		super( name.name, name.token.beginLine );
		addType( Except.class );
		
		if (args.length > 1)
			throw new ParseException( String.format(
				"Unchecked accepts one optional boolean argument at line %d",
				name.token.beginLine ) );
		
		if (args.length > 0)
		{
			Token arg = args[0];
			if (arg.kind != EtchGrammarConstants.TRUE &&
					arg.kind != EtchGrammarConstants.FALSE)
				throw new ParseException( String.format(
					"Unchecked accepts one optional boolean argument at line %d",
					name.token.beginLine ) );
			unchecked = Boolean.parseBoolean( arg.image );
		}
		else
		{
			unchecked = true;
		}
	}
	
	private final boolean unchecked;
	
	/**
	 * @return true if the exception should inherit from an unchecked
	 * type (e.g., java's RuntimeException) vs. a checked type (e.g.,
	 * java's Exception).
	 */
	public boolean isUnchecked()
	{
		return unchecked;
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%s@Unchecked\n", indent );
	}

	@Override
	public String toString()
	{
		return String.format( "@Unchecked" );
	}
}
