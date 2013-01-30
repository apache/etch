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
import org.apache.etch.compiler.ast.Message;
import org.apache.etch.compiler.ast.Name;
import org.apache.etch.compiler.ast.Opt;
import org.apache.etch.compiler.ast.Service;


/**
 * Option which sets the timeout of a message which is expecting a
 * result (that is, a call).
 * The timeout is in ms.
 */
public class Timeout extends Opt
{
	/**
	 * Constructs the Timeout.
	 *
	 * @param name
	 * @param args
	 * @throws ParseException
	 */
	public Timeout( Name name, Token... args ) throws ParseException
	{
		super( name.name, name.token.beginLine );
		addType( Message.class );
		addType( Service.class );
		
		if (args.length != 1)
			throw new ParseException( String.format( "args.length != 1" ) );
		Token arg = args[0];
		if (arg.kind != EtchGrammarConstants.INTEGER)
			throw new ParseException( String.format( "Timeout arg should be integer constant: "+arg.image ) );
		int v = Integer.parseInt( arg.image );
		if (v < 0)
			throw new ParseException( String.format( "Timeout arg should be integer constant >= 0: "+arg.image ) );
		
		timeout = v;
	}
	
	private final int timeout;
	
	/**
	 * @return the timeout value.
	 */
	public int timeout()
	{
		return timeout;
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%s@Timeout( %d )\n", indent, timeout );
	}

	@Override
	public String toString()
	{
		return String.format( "@Timeout( %d )", timeout );
	}
}
