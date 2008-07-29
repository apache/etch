/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.opt;

import etch.compiler.EtchGrammarConstants;
import etch.compiler.ParseException;
import etch.compiler.Token;
import etch.compiler.ast.Message;
import etch.compiler.ast.Name;
import etch.compiler.ast.Opt;
import etch.compiler.ast.Service;

/**
 * Option which sets the oneway attribute of a message. If the message is
 * oneway, then remote sends a message and doesn't wait for any response or
 * thrown exception.
 */
public class Oneway extends Opt
{
	/**
	 * Constructs the Oneway.
	 *
	 * @param name
	 * @param args
	 * @throws ParseException 
	 */
	public Oneway( Name name, Token... args ) throws ParseException
	{
		super( name.name, name.token.beginLine );
		addType( Message.class );
		addType( Service.class );
		
		if (args.length > 1)
			throw new ParseException( String.format(
				"Oneway accepts one optional boolean argument at line %d",
				name.token.beginLine ) );
		
		if (args.length > 0)
		{
			Token arg = args[0];
			if (arg.kind != EtchGrammarConstants.TRUE &&
					arg.kind != EtchGrammarConstants.FALSE)
				throw new ParseException( String.format(
					"Oneway accepts one optional boolean argument at line %d",
					name.token.beginLine ) );
			oneway = Boolean.parseBoolean( arg.image );
		}
		else
		{
			oneway = true;
		}
	}
	
	private final boolean oneway;
	
	/**
	 * @return true if the message is a oneway message. A oneway message
	 * does not delay the sender waiting for a response.
	 */
	public boolean isOneway()
	{
		return oneway;
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%s@Oneway %s\n", indent, oneway );
	}

	@Override
	public String toString()
	{
		return String.format( "@Oneway %s", oneway );
	}
}
