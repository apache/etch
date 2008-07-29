/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import etch.compiler.Backend;
import etch.compiler.EtchGrammarConstants;
import etch.compiler.ParseException;
import etch.compiler.Token;

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
		if (name().token.kind != EtchGrammarConstants.ID)
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
		Token token = name().token;
		if (token.kind == EtchGrammarConstants.ID)
		{
			Named<?> n = getNamed();
			if (n == null)
				throw new ParseException( String.format(
					"undefined exception %s at %d", token.image, token.beginLine ) );
	
			if (n instanceof Except)
				return;
			
			throw new ParseException( String.format(
				"%s not an exception at %d", token.image, token.beginLine ) );
		}
		
		throw new ParseException( String.format(
			"%s not an exception at %d", token.image, token.beginLine ) );
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.doThrown( this );
	}
}
