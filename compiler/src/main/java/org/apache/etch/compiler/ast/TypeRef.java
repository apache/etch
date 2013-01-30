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

import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.Token;

/**
 * A type reference from the etch idl.
 */
public class TypeRef
{
	/**
	 * Constructs the TypeRef.
	 * @param intf 
	 *
	 * @param type
	 */
	public TypeRef( Service intf, Token type )
	{
		this.intf = intf;
		this.type = type;
	}
	
	private final Service intf;
	
	private final Token type;
	
	/**
	 * @return the parent object of this reference.
	 */
	public Service intf()
	{
		return intf;
	}
	
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
		dim = d;
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
				"undefined or ambiguous type %s at line %d", type.image, type.beginLine ) );

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
	
	/**
	 * Call the tree walker on our sub-elements.
	 * @param walker
	 */
	public void treewalk( TreeWalker walker )
	{
		walker.doTypeRef( this );
	}

	/**
	 * @return true if the referenced type is builtin (currently all builtins
	 * use keywords to reference them).
	 */
	public boolean isBuiltin()
	{
		return type.kind != EtchGrammarConstants.ID
			&& type.kind != EtchGrammarConstants.QID;
	}

	/**
	 * @return the dimensionality of this type reference.
	 */
	public int dim()
	{
		return dim;
	}

	/**
	 * @return true if this type reference is an array.
	 */
	public boolean isArray()
	{
		return dim > 0;
	}
	
	/**
	 * @return true if this type reference is a scalar boolean.
	 */
	public boolean isBoolean()
	{
		return type.kind == EtchGrammarConstants.BOOLEAN && !isArray();
	}

	/**
	 * @return true if this type reference is a scalar integral type.
	 */
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

	/**
	 * @return true if this type reference is a scalar floating type.
	 */
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

	/**
	 * @return true if this type reference is a scalar string type.
	 */
	public boolean isString()
	{
		if (isArray())
			return false;

		if (type.kind == EtchGrammarConstants.STRING)
			return true;
		
		return false;
	}
	
	/**
	 * @return true if this type reference is a scalar object type.
	 */
	public boolean isObject()
	{
		if (isArray())
			return false;

		if (type.kind == EtchGrammarConstants.OBJECT)
			return true;
		
		return false;
	}
}
