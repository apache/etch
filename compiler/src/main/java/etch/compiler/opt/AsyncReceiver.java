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
import etch.compiler.ast.AsyncReceiverMode;
import etch.compiler.ast.Message;
import etch.compiler.ast.Name;
import etch.compiler.ast.Opt;
import etch.compiler.ast.Service;

/**
 * Option which sets the async mode of the message receiver.
 */
public class AsyncReceiver extends Opt
{
	/**
	 * Constructs the AsyncReceiver.
	 *
	 * @param name
	 * @param args
	 * @throws ParseException
	 */
	@SuppressWarnings("deprecation")
	public AsyncReceiver( Name name, Token... args ) throws ParseException
	{
		super( name.name, name.token.beginLine );
		addType( Message.class );
		addType( Service.class );
		
		if (args.length != 1)
			throw new ParseException( String.format( "AsyncReceiver args length != 1" ) );
		
		Token arg = args[0];
		if (arg.kind != EtchGrammarConstants.ID)
			throw new ParseException( String.format( "AsyncReceiver arg should be identifier: "+arg.image ) );
		
		try
		{
			AsyncReceiverMode m = AsyncReceiverMode.valueOf( arg.image.toUpperCase() );
			if (m == AsyncReceiverMode.POOL)
			{
				System.out.printf(
					"WARNING: deprecated AsyncReceiver mode POOL should be changed to QUEUED at line %d\n",
					arg.beginLine );
				m = AsyncReceiverMode.QUEUED;
			}
			this.mode = m;
		}
		catch ( IllegalArgumentException e )
		{
			throw new ParseException( String.format(
				"AsyncReceiver arg should be one of: "+EnumSet.allOf( AsyncReceiverMode.class ) ) );
		}
	}
	
	private final AsyncReceiverMode mode;

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%s@AsyncReceiver( %s )\n", indent, mode );
	}

	@Override
	public String toString()
	{
		return String.format( "@AsyncReceiver( %s )", mode );
	}

	/**
	 * @return the async receiver mode.
	 */
	public AsyncReceiverMode getMode()
	{
		return mode;
	}
}
