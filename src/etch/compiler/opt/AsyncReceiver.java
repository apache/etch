/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
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
