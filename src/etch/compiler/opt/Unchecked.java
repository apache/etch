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
import etch.compiler.ast.Except;
import etch.compiler.ast.Name;
import etch.compiler.ast.Opt;

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
