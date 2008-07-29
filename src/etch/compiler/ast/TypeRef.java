/* $Id$
 *
 * Created by sccomer on Apr 24, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import etch.compiler.EtchGrammarConstants;
import etch.compiler.ParseException;
import etch.compiler.Token;

/**
 * A type reference from the etch idl.
 */
public class TypeRef
{
	/**
	 * Constructs the TypeRef.
	 *
	 * @param type
	 */
	public TypeRef( Token type )
	{
		this.type = type;
	}
	
	private final Token type;
	
	/**
	 * @return the token of the type.
	 */
	public Token type()
	{
		return type;
	}

	/**
	 * Sets the number of dimensions of an array type.
	 * @param d the number of dimensions of an array type.
	 */
	public void setDim( int d )
	{
		this.dim = d;
	}
	
	private int dim;

	@Override
	public String toString()
	{
		StringBuffer sb = new StringBuffer( type.image );
		for (int i = 0; i < dim; i++)
			sb.append( "[]" );
		return sb.toString();
	}
	
	/**
	 * @param i
	 * @return the named type.
	 */
	public Named<?> getNamed( Service i )
	{
		if (isBuiltin())
			return null;
		
		return i.get( type.image );
	}

	/**
	 * Checks the referenced type to make sure it is defined.
	 * @param i
	 * @throws ParseException
	 */
	public void check( Service i ) throws ParseException
	{
		if (isBuiltin())
			return;

		Named<?> n = getNamed( i );
		
		if (n == null)
			throw new ParseException( String.format(
				"undefined type %s at line %d", type.image, type.beginLine ) );

		if (n instanceof Enumx)
			return;
		
		if (n instanceof Struct)
			return;
		
		if (n instanceof Extern)
			return;
		
		if (n instanceof Builtin)
			return;
		
		throw new ParseException( String.format(
			"%s not a type at line %d", type.image, type.beginLine ) );
	}
	
	public void treewalk( TreeWalker walker )
	{
		walker.doTypeRef( this );
	}

	public boolean isBuiltin()
	{
		return type.kind != EtchGrammarConstants.ID;
	}

	/**
	 * @return the dimensionality of this type reference.
	 */
	public int dim()
	{
		return dim;
	}

	public boolean isArray()
	{
		return dim > 0;
	}
	
	public boolean isBoolean()
	{
		return type.kind == EtchGrammarConstants.BOOLEAN && !isArray();
	}

	public boolean isIntegral()
	{
		if (isArray())
			return false;

		if (type.kind == EtchGrammarConstants.BYTE)
			return true;
		
		if (type.kind == EtchGrammarConstants.SHORT)
			return true;
		
		if (type.kind == EtchGrammarConstants.INT)
			return true;
		
		if (type.kind == EtchGrammarConstants.LONG)
			return true;
		
		return false;
	}

	public boolean isFloating()
	{
		if (isArray())
			return false;

		if (type.kind == EtchGrammarConstants.FLOAT)
			return true;
		
		if (type.kind == EtchGrammarConstants.DOUBLE)
			return true;
		
		return false;
	}

	public boolean isString()
	{
		if (isArray())
			return false;

		if (type.kind == EtchGrammarConstants.STRING)
			return true;
		
		return false;
	}
	
	public boolean isObject()
	{
		if (isArray())
			return false;

		if (type.kind == EtchGrammarConstants.OBJECT)
			return true;
		
		return false;
	}
}
