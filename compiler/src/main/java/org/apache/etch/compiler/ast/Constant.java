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

import java.util.Map;

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.Token;


/**
 * A named constant declaration from the etch idl.
 */
public class Constant extends Named<Service>
{
	/**
	 * Constructs the Constant.
	 *
	 * @param intf
	 * @param name
	 * @param nOpts
	 * @param type
	 * @param value
	 */
	public Constant( Service intf, Name name, Map<String, Opt> nOpts,
		TypeRef type, Token value )
	{
		super( intf, name, nOpts );
		this.type = type;
		this.value = value;
	}
	
	private final TypeRef type;
	
	private final Token value;
	
	/**
	 * @return the type of this constant.
	 */
	public TypeRef type()
	{
		return type;
	}
	
	/**
	 * @return the value of this constant.
	 */
	public Token value()
	{
		return value;
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
	
//	@Override
//	public Integer id()
//	{
//		return null;
//	}
	
	@Override
	public String toString()
	{
		return String.format( "const %s %s = %s", type, name().name, value );
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%sconst %s %s = %s\n", indent, type, name().name, value );
		
		String sIndent = indent+"  ";
		
		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );
	}

	@Override
	public void check() throws ParseException
	{
		checkConstantName( name() );
		
		// types: boolean, byte, short, int, long, float, double, string
		// values: true, false, integer, octal, hex, binary, decimal, str
		
		switch (type.type().kind)
		{
			case EtchGrammarConstants.BOOLEAN:
				if (!isBoolean( value ))
					incompatValueForType( type, value );
				break;
			case EtchGrammarConstants.BYTE:
				if (!isByte( value ))
					incompatValueForType( type, value );
				break;
			case EtchGrammarConstants.SHORT:
				if (!isShort( value ))
					incompatValueForType( type, value );
				break;
			case EtchGrammarConstants.INT:
				if (!isInt( value ))
					incompatValueForType( type, value );
				break;
			case EtchGrammarConstants.LONG:
				if (!isLong( value ))
					incompatValueForType( type, value );
				break;
			case EtchGrammarConstants.FLOAT:
				if (!isFloat( value ))
					incompatValueForType( type, value );
				break;
			case EtchGrammarConstants.DOUBLE:
				if (!isDouble( value ))
					incompatValueForType( type, value );
				break;
			case EtchGrammarConstants.STRING:
				if (!isString( value ))
					incompatValueForType( type, value );
				break;
			default:
				throw new ParseException( "unknown type "+type.type() );
		}
	}

	private void incompatValueForType( TypeRef t, Token v ) throws ParseException
	{
		throw new ParseException( String.format(
			"incompatible value for type %s at line %d: %s",
			type.type(), value.beginLine, value ) );
	}

	private boolean isBoolean( Token v )
	{
		if (v.kind == EtchGrammarConstants.TRUE)
			return true;
		if (v.kind == EtchGrammarConstants.FALSE)
			return true;
		return false;
	}

	private boolean isByte( Token v )
	{
		if (v.kind == EtchGrammarConstants.INTEGER)
			return checkByte( v.image, 10 );
		if (v.kind == EtchGrammarConstants.OCTAL)
			return v.image.startsWith( "0" ) && checkByte( v.image.substring( 1 ), 8 );
		if (v.kind == EtchGrammarConstants.HEX)
			return v.image.toLowerCase().startsWith( "0x" ) && checkByte( v.image.substring( 2 ), 16 );
		if (v.kind == EtchGrammarConstants.BINARY)
			return v.image.toLowerCase().startsWith( "0b" ) && checkByte( v.image.substring( 2 ), 2 );
		return false;
	}

	private boolean checkByte( String s, int radix )
	{
		if (radix == 10)
			return checkNumber( s, radix, -128, 127 );
		return checkNumber( s, radix, 0, 255 );
	}

	private boolean isShort( Token v )
	{
		if (v.kind == EtchGrammarConstants.INTEGER)
			return checkShort( v.image, 10 );
		if (v.kind == EtchGrammarConstants.OCTAL)
			return v.image.startsWith( "0" ) && checkShort( v.image.substring( 1 ), 8 );
		if (v.kind == EtchGrammarConstants.HEX)
			return v.image.toLowerCase().startsWith( "0x" ) && checkShort( v.image.substring( 2 ), 16 );
		if (v.kind == EtchGrammarConstants.BINARY)
			return v.image.toLowerCase().startsWith( "0b" ) && checkShort( v.image.substring( 2 ), 2 );
		return false;
	}

	private boolean checkShort( String s, int radix )
	{
		if (radix == 10)
			return checkNumber( s, radix, -32768, 32767 );
		return checkNumber( s, radix, 0, 65535 );
	}

	private boolean isInt( Token v )
	{
		if (v.kind == EtchGrammarConstants.INTEGER)
			return checkInt( v.image, 10 );
		if (v.kind == EtchGrammarConstants.OCTAL)
			return v.image.startsWith( "0" ) && checkInt( v.image.substring( 1 ), 8 );
		if (v.kind == EtchGrammarConstants.HEX)
			return v.image.toLowerCase().startsWith( "0x" ) && checkInt( v.image.substring( 2 ), 16 );
		if (v.kind == EtchGrammarConstants.BINARY)
			return v.image.toLowerCase().startsWith( "0b" ) && checkInt( v.image.substring( 2 ), 2 );
		return false;
	}

	private boolean checkInt( String s, int radix )
	{
		if (radix == 10)
			return checkNumber( s, radix, -2147483648, 2147483647 );
		return checkNumber( s, radix, 0, 4294967295L );
	}

	private boolean isLong( Token v )
	{
		if (v.kind == EtchGrammarConstants.INTEGER)
			return checkLong( v.image, 10 );
		if (v.kind == EtchGrammarConstants.OCTAL)
			return v.image.startsWith( "0" ) && checkLong( v.image.substring( 1 ), 8 );
		if (v.kind == EtchGrammarConstants.HEX)
			return v.image.toLowerCase().startsWith( "0x" ) && checkLong( v.image.substring( 2 ), 16 );
		if (v.kind == EtchGrammarConstants.BINARY)
			return v.image.toLowerCase().startsWith( "0b" ) && checkLong( v.image.substring( 2 ), 2 );
		return false;
	}

	private boolean checkLong( String s, int radix )
	{
		if (radix == 10)
			return checkNumber( s, radix, -9223372036854775808L, 9223372036854775807L );
		return checkNumber( s, radix, 0, -1L );
	}

	private boolean checkNumber( String s, int radix, long min, long max )
	{
		// TODO this needs to work better for non-decimal radix
		try
		{
			long l = Long.parseLong( s, radix );
			return l >= min && l <= max;
		}
		catch ( NumberFormatException e )
		{
			return false;
		}
	}

	private boolean isFloat( Token v )
	{
		// TODO implement some sort of float value checking...
		if (v.kind == EtchGrammarConstants.INTEGER)
			return true;
		if (v.kind == EtchGrammarConstants.OCTAL)
			return true;
		if (v.kind == EtchGrammarConstants.HEX)
			return true;
		if (v.kind == EtchGrammarConstants.BINARY)
			return true;
		if (v.kind == EtchGrammarConstants.DECIMAL)
			return true;
		return false;
	}

	private boolean isDouble( Token v )
	{
		// TODO implement some sort of double value checking...
		if (v.kind == EtchGrammarConstants.INTEGER)
			return true;
		if (v.kind == EtchGrammarConstants.OCTAL)
			return true;
		if (v.kind == EtchGrammarConstants.HEX)
			return true;
		if (v.kind == EtchGrammarConstants.BINARY)
			return true;
		if (v.kind == EtchGrammarConstants.DECIMAL)
			return true;
		return false;
	}

	private boolean isString( Token v )
	{
		if (v.kind == EtchGrammarConstants.STR)
			return true;
		return false;
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.preConstant( this );
		
		type.treewalk( walker );
		
		walker.postConstant( this );
	}
}
