/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.compiler.opt;

import java.util.EnumSet;

import etch.compiler.EtchGrammarConstants;
import etch.compiler.ParseException;
import etch.compiler.Token;
import etch.compiler.ast.Message;
import etch.compiler.ast.MessageDirection;
import etch.compiler.ast.Name;
import etch.compiler.ast.Opt;
import etch.compiler.ast.Service;

/**
 * Option which sets the direction of a message ([client->]server,
 * [server->]client, or both.
 */
public class Direction extends Opt
{
	/**
	 * Constructs the Direction.
	 *
	 * @param name
	 * @param args
	 * @throws ParseException
	 */
	public Direction( Name name, Token... args ) throws ParseException
	{
		super( name.name, name.token.beginLine );
		addType( Message.class );
		addType( Service.class );
		
		if (args.length != 1)
			throw new ParseException( String.format( "Direction args length != 1" ) );
		
		Token arg = args[0];
		if (arg.kind != EtchGrammarConstants.ID)
			throw new ParseException( String.format( "Direction arg should be identifier: "+arg.image ) );
		
		try
		{
			this.md = MessageDirection.valueOf( arg.image.toUpperCase() );
		}
		catch ( IllegalArgumentException e )
		{
			throw new ParseException( String.format(
				"Direction arg should be one of: "+EnumSet.allOf( MessageDirection.class ) ) );
		}
	}
	
	private final MessageDirection md;

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%s@Direction( %s )\n", indent, md );
	}

	@Override
	public String toString()
	{
		return String.format( "@Direction( %s )", md );
	}

	/**
	 * @return the appropriate MessageDirection for the direction.
	 */
	public MessageDirection getMessageDirection()
	{
		return md;
	}
}
