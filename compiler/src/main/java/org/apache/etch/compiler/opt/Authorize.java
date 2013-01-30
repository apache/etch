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

import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.Token;
import org.apache.etch.compiler.ast.Constant;
import org.apache.etch.compiler.ast.Enumx;
import org.apache.etch.compiler.ast.Item;
import org.apache.etch.compiler.ast.Message;
import org.apache.etch.compiler.ast.Name;
import org.apache.etch.compiler.ast.Named;
import org.apache.etch.compiler.ast.Opt;
import org.apache.etch.compiler.ast.Parameter;
import org.apache.etch.compiler.ast.Service;
import org.apache.etch.compiler.ast.Struct;
import org.apache.etch.compiler.ast.TypeRef;
import org.apache.etch.util.StringUtil;


/**
 * Option which sets the authorization method for a message.
 */
public class Authorize extends Opt
{
	/**
	 * Constructs the Authorize.
	 *
	 * @param name
	 * @param args
	 * @throws ParseException
	 */
	public Authorize( Name name, Token... args ) throws ParseException
	{
		super( name.name, name.token.beginLine );
		addType( Message.class );
		addType( Service.class );
		
		if (args == null || args.length == 0)
		{
			method = new Token();
			method.kind = EtchGrammarConstants.TRUE;
			method.beginLine = name.token.beginLine;
			argList = new ArrayList<AuthArg>();
			return;
		}
		
		method = args[0];
		if (method.kind != EtchGrammarConstants.ID &&
				method.kind != EtchGrammarConstants.TRUE &&
				method.kind != EtchGrammarConstants.FALSE)
		{
			throw new ParseException( String.format(
				"Authorize method should be identifier, true, or false (but not %s) at line %d",
				method, method.beginLine ) );
		}
		
		if ((method.kind == EtchGrammarConstants.TRUE ||
				method.kind == EtchGrammarConstants.FALSE) && args.length > 1)
		{
			throw new ParseException( String.format(
				"Authorize %d cannot have any args at line %d",
				method, method.beginLine ) );
		}

		argList = argsToArgList( args );
	}
	
	private final Token method;
	
	private final List<AuthArg> argList;
	
	/**
	 * @return the authorization method.
	 */
	public Token method()
	{
		return method;
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%s@Authorize( %s, %s )\n", indent, method, argList );
	}

	@Override
	public String toString()
	{
		return String.format( "@Authorize( %s, %s )", method, argList );
	}

	/**
	 * @return args as a list.
	 */
	private List<AuthArg> argsToArgList( Token[] args )
	{
		List<AuthArg> list = new ArrayList<AuthArg>();
		int n = args.length;
		for (int i = 1; i < n; i++)
			list.add( new AuthArg( args[i] ) );
		return list;
	}
	
	/**
	 * @return the args for the authorization method.
	 */
	public List<AuthArg> args()
	{
		return argList;
	}

	/**
	 * @return true if the message needs authorization.
	 */
	public boolean hasAuth()
	{
		return method != null && method.kind != EtchGrammarConstants.TRUE;
	}

	/**
	 * Checks the method to be called by authorize to make sure it is
	 * a service method and that the parameters are correct.
	 * @param service our service definition
	 * @param msg the message being protected by the auth
	 * @throws ParseException
	 */
	public void check( Service service, Message msg ) throws ParseException
	{
		if (!hasAuth())
			return;
		
		if (isMethodTrue() || isMethodFalse())
			return;
		
		Named<?> named = service.get( method.image );
		if (named == null)
			throw new ParseException( String.format(
				"Authorize method name %s is not defined at %d",
					method.image, lineno() ) );
		
		if (!named.isMessage())
			throw new ParseException( String.format(
				"Authorize method name %s is not a method at %d",
				method.image, lineno() ) );
		
		Message authMsg = (Message) named;
		
		if (authMsg.type().type().kind != EtchGrammarConstants.BOOLEAN)
			throw new ParseException( String.format(
				"Authorize method %s result type is not boolean at %d",
				method.image, lineno() ) );
		
		List<Parameter> authParams = authMsg.getParameters();
		if (authParams.size() != argList.size())
			throw new ParseException( String.format(
				"Authorize method %s parameter list size does not match the number of supplied arguments at %d",
				method.image, lineno() ) );
		
		int n = argList.size();
		for (int i = 0; i < n; i++)
		{
			Parameter param = authParams.get( i );
			AuthArg aarg = argList.get( i );
			aarg.setType( param.type() );
			Token arg = aarg.value();
			
//			System.out.printf( "auth method %s param %s = %s\n",
//				method, param.name(), arg );
			
			switch (arg.kind)
			{
				case EtchGrammarConstants.NULL:
					// any parameter value may be specified as null...
					break;
				
				case EtchGrammarConstants.ID:
					// arg is referencing either a parameter of msg or a
					// service constant.
					checkMsgParamOrServiceConst( msg, param, arg, i );
					break;
				
				case EtchGrammarConstants.QID:
					// arg is referencing an enum item (enum.item) or
					// could be trying to reference a field of a
					// parameter of message (param(.field)+).
					checkMsgParamFieldRefOrEnumItem( msg, param, arg, i );
					break;
				
				case EtchGrammarConstants.TRUE:
				case EtchGrammarConstants.FALSE:
					// arg is a boolean constant the parameter better be
					// boolean, too.
					checkBooleanConstant( param, arg, i );
					break;
				
				case EtchGrammarConstants.INTEGER:
					// arg is an integer constant, the parameter better
					// be an integral or floating type and arg better
					// be the right size...
					checkIntegerConstant( param, arg, i );
					break;
				
				case EtchGrammarConstants.OCTAL:
					// arg is an octal constant, the parameter better
					// be an integral or floating type and arg better
					// be the right size...
					checkOctalConstant( param, arg, i );
					break;
				
				case EtchGrammarConstants.HEX:
					// arg is a hex constant, the parameter better
					// be an integral or floating type and arg better
					// be the right size...
					checkHexConstant( param, arg, i );
					break;
				
				case EtchGrammarConstants.BINARY:
					// arg is a binary constant, the parameter better
					// be an integral or floating type and arg better
					// be the right size...
					checkBinaryConstant( param, arg, i );
					break;
				
				case EtchGrammarConstants.DECIMAL:
					// arg is a decimal constant, the parameter better
					// be a floating type and arg better be the right
					// size...
					checkDecimalConstant( param, arg, i );
					break;
				
				case EtchGrammarConstants.STR:
					// arg is a string constant the parameter better be
					// string, too.
					checkStringConstant( param, arg, i );
					break;
				
				default:
					throw new ParseException( String.format(
						"Authorize method %s arg %d unsupported kind (%s) at line %d",
						method.image, i+1, arg, lineno() ) );
			}
		}
	}

	/**
	 * @return true if the method is false.
	 */
	public boolean isMethodFalse()
    {
//		System.out.println( "isMethodFalse() => "+(method.kind == EtchGrammarConstants.FALSE) );
	    return method.kind == EtchGrammarConstants.FALSE;
    }

	/**
	 * @return true if the method is true.
	 */
	public boolean isMethodTrue()
    {
	    return method.kind == EtchGrammarConstants.TRUE;
    }

	private void checkMsgParamOrServiceConst( Message msg, Parameter param,
		Token arg, int argNo ) throws ParseException
	{
		Parameter p = msg.getParameter( arg.image );
		if (p != null)
		{
			// it's a parameter, do the types match?
			
			Named<?> pt = p.type().getNamed( msg.parent() );
			Named<?> paramType = param.type().getNamed( msg.parent() );
			if (pt == paramType)
				return;
			
			throw typeMismatch( param, arg, argNo );
		}
		
		Named<?> named = msg.parent().get( arg.image );
		
		if (named != null)
		{
			// it's a named thing, it is a constant and do the
			// types match?
			
			if (named.isConstant())
			{
				Constant c = (Constant) named;
				
				// it's a constant, do the types match?
				
				Named<?> ct = c.type().getNamed( msg.parent() );
				Named<?> paramType = param.type().getNamed( msg.parent() );
				if (ct == paramType)
					return;
				
				throw typeMismatch( param, arg, argNo );
			}
			
			// the named thing is not a constant, so blow chunks.
			
			throw new ParseException( String.format(
				"Authorize method %s arg %d name not a parameter or constant (%s) at line %d",
				method.image, argNo+1, arg, lineno() ) );
		}
		
		// the named thing does not exist, so blow chunks.
		
		throw new ParseException( String.format(
			"Authorize method %s arg %d name unknown (%s) at line %d",
			method.image, argNo+1, arg, lineno() ) );
	}

	private void checkMsgParamFieldRefOrEnumItem( Message msg, Parameter param,
		Token arg, int argNo ) throws ParseException
	{
		Service service = msg.parent();
		
		StringTokenizer path = new StringTokenizer( arg.image, "." );
		String name = path.nextToken();
		
		Parameter p = msg.getParameter( name );
		if (p != null)
		{
			// it's a parameter, so type of p must be a struct...

			p = getField( service, p, path );
			
			if (p == null)
				throw new ParseException( String.format(
					"Authorize method %s arg %d name does not resolve to a field (%s) at line %d",
					method.image, argNo+1, arg, lineno() ) );
			
			Named<?> pt = p.type().getNamed( service );
			Named<?> paramType = param.type().getNamed( service );
			if (pt == paramType)
				return;

			throw typeMismatch( param, arg, argNo );
		}
		
		// it isn't a parameter, so it must be an enum...
		
		Named<?> named = service.get( name );
		if (named == null)
			throw new ParseException( String.format(
				"Authorize method %s arg %d name unknown (%s) at line %d",
				method.image, argNo+1, arg, lineno() ) );
		
		if (!named.isEnumx())
			throw new ParseException( String.format(
				"Authorize method %s arg %d name not a parameter field or enum (%s) at line %d",
				method.image, argNo+1, arg, lineno() ) );
		
		Enumx e = (Enumx) named;
		
		Item i = e.getItem( path.nextToken() );
		if (i == null)
			throw new ParseException( String.format(
				"Authorize method %s arg %d name not an enum item (%s) at line %d",
				method.image, argNo+1, arg, lineno() ) );
		
		if (path.hasMoreTokens())
			throw new ParseException( String.format(
				"Authorize method %s arg %d name not an enum item (%s) at line %d",
				method.image, argNo+1, arg, lineno() ) );
	}

	/**
	 * @param service
	 * @param p
	 * @param path
	 * @return the parameter that was resolved using the tokens from st.
	 */
	private Parameter getField( Service service, Parameter p, StringTokenizer path )
	{
		if (!path.hasMoreTokens())
			return p;
		
		String name = path.nextToken();
		
		// get the type of p, it better be a struct.
		
		Named<?> named = p.type().getNamed( service );
		if (named == null || !named.isStruct())
			return null;
		
		Struct s = (Struct) named;
		
		p = s.getParameter( name );
		if (p == null)
			return null;
		
		return getField( service, p, path );
	}

	private void checkBooleanConstant( Parameter param, Token arg, int argNo )
		throws ParseException
	{
		if (!param.type().isBoolean())
			throw typeMismatch( param, arg, argNo );
	}

	private ParseException typeMismatch( Parameter param, Token arg, int argNo )
	{
		//if (true) throw new RuntimeException( "type mismatch" );
		return new ParseException( String.format(
			"Authorize method %s parameter %d (%s) type mismatch at line %d",
			method, argNo+1, param.name(), arg.beginLine ) );
	}

	private void checkIntegerConstant( Parameter param, Token arg, int argNo )
		throws ParseException
	{
		if (!(param.type().isIntegral() || param.type().isFloating()))
			throw typeMismatch( param, arg, argNo );
		
		Long value = parseLong( arg.image, 10, "" );
		if (value != null)
		{
			// check range of long vs. integral or floating param type
			if (checkRange( value, param, arg, argNo ))
				return;
		}
		
		// value range is too much for long type, or parameter is not a
		// known integral or floating type; try double
		
		Double dvalue = parseDouble( arg.image );
		if (dvalue != null)
		{
			// check range of double vs. floating param type
			if (checkRange( dvalue, param, arg, argNo ))
				return;
		}
		
		// value range is too much for long or double type, or parameter
		// is not a known integral or floating type
		
		throw typeMismatch( param, arg, argNo );
	}

	private boolean checkRange( long value, Parameter param, Token arg,
		int argNo ) throws ParseException
	{
		// we parsed the value as a long, now see if it is compatible with
		// our parameter type.
		switch (param.type().type().kind)
		{
			case EtchGrammarConstants.BYTE:
				checkRange( value, Byte.MIN_VALUE, Byte.MAX_VALUE, param, arg,
					argNo );
				return true;
			
			case EtchGrammarConstants.SHORT:
				checkRange( value, Short.MIN_VALUE, Short.MAX_VALUE, param, arg,
					argNo );
				return true;
			
			case EtchGrammarConstants.INT:
				checkRange( value, Integer.MIN_VALUE, Integer.MAX_VALUE, param,
					arg, argNo );
				return true;
			
			case EtchGrammarConstants.LONG:
			case EtchGrammarConstants.FLOAT:
			case EtchGrammarConstants.DOUBLE:
				// value is a long, and that is certainly compatible with long,
				// float, or double.
				return true;
			
			default:
				// parameter is not a recognized type.
				return false;
		}
	}

	private boolean checkRange( double dvalue, Parameter param, Token arg,
		int argNo ) throws ParseException
	{
		// we parsed the value as a double, now see if it is compatible with
		// our parameter type.
		switch (param.type().type().kind)
		{
			case EtchGrammarConstants.FLOAT:
				checkRange( dvalue, Float.MIN_VALUE, Float.MAX_VALUE, param,
					arg, argNo );
				return true;
			
			case EtchGrammarConstants.DOUBLE:
				// value is a double, and that is certainly compatible with
				// double.
				return true;
			
			default:
				// parameter is not a recognized type.
				return false;
		}
	}

	private void checkRange( long value, long min, long max, Parameter param,
		Token arg, int argNo ) throws ParseException
	{
		if (value >= min && value <= max)
			return;
		
		throw new ParseException( String.format(
			"Authorize method %s parameter %d (%s) integral value out of range at line %d",
			method, argNo+1, param.name(), arg.beginLine ) );
	}

	private void checkRange( double dvalue, double min, double max,
		Parameter param, Token arg, int argNo ) throws ParseException
	{
		// ignore sign, it doesn't matter.
		
		dvalue = Math.abs( dvalue );
		
		if (dvalue >= min && dvalue <= max)
			return;
		
		throw new ParseException( String.format(
			"Authorize method %s parameter %d (%s) floating value out of range at line %d",
			method, argNo+1, param.name(), arg.beginLine ) );
	}

	private Long parseLong( String s, int radix, String prefix )
	{
		try
		{
			if (!s.toLowerCase().startsWith( prefix ))
				return null;
			return Long.parseLong( s.substring( prefix.length() ), radix );
		}
		catch ( NumberFormatException e )
		{
			return null;
		}
	}

	private Double parseDouble( String image )
	{
		try
		{
			return Double.parseDouble( image );
		}
		catch ( NumberFormatException e )
		{
			return null;
		}
	}

	private void checkOctalConstant( Parameter param, Token arg, int argNo )
		throws ParseException
	{
		if (!param.type().isIntegral())
			throw typeMismatch( param, arg, argNo );
		
		Long value = parseLong( arg.image, 8, "0" );
		if (value != null)
		{
			// check range of long vs. integral or floating param type
			if (checkRange( value, param, arg, argNo ))
				return;
		}
		
		// value range is too much for long or double type, or parameter
		// is not a known integral or floating type
		
		throw typeMismatch( param, arg, argNo );
	}

	private void checkHexConstant( Parameter param, Token arg, int argNo )
		throws ParseException
	{
		if (!param.type().isIntegral())
			throw typeMismatch( param, arg, argNo );
		
		Long value = parseLong( arg.image, 16, "0x" );
		if (value != null)
		{
			// check range of long vs. integral or floating param type
			if (checkRange( value, param, arg, argNo ))
				return;
		}
		
		// value range is too much for long or double type, or parameter
		// is not a known integral or floating type
		
		throw typeMismatch( param, arg, argNo );
	}

	private void checkBinaryConstant( Parameter param, Token arg, int argNo )
		throws ParseException
	{
		if (!param.type().isIntegral())
			throw typeMismatch( param, arg, argNo );
		
		Long value = parseLong( arg.image, 2, "0b" );
		if (value != null)
		{
			// check range of long vs. integral or floating param type
			if (checkRange( value, param, arg, argNo ))
				return;
		}
		
		// value range is too much for long or double type, or parameter
		// is not a known integral or floating type
		
		throw typeMismatch( param, arg, argNo );
	}

	private void checkDecimalConstant( Parameter param, Token arg, int argNo )
		throws ParseException
	{
		if (!param.type().isFloating())
			throw typeMismatch( param, arg, argNo );
		
		Double dvalue = parseDouble( arg.image );
		if (dvalue != null)
		{
			// check range of double vs. floating param type
			if (checkRange( dvalue, param, arg, argNo ))
				return;
		}
		
		// value range is too much for long or double type, or parameter
		// is not a known integral or floating type
		
		throw typeMismatch( param, arg, argNo );
	}

	private void checkStringConstant( Parameter param, Token arg, int argNo )
		throws ParseException
	{
		if (!param.type().isString())
			throw typeMismatch( param, arg, argNo );
	}
	
	/**
	 * Wrapper for arg which holds the required arg type as well.
	 */
	public static class AuthArg
	{
		/**
		 * @param arg
		 */
		public AuthArg( Token arg )
		{
			this.arg = arg;
		}
		
		private final Token arg;
		
		private TypeRef type;
		
		/**
		 * @return the token of the arg.
		 */
		public Token value()
		{
			return arg;
		}
		
		/**
		 * Sets the required type of the arg.
		 * @param type
		 */
		public void setType( TypeRef type )
		{
			this.type = type;
		}
		
		/**
		 * @return the required type of the arg.
		 */
		public TypeRef type()
		{
			return type;
		}
		
		/**
		 * @return true if not an id or qid.
		 */
		public boolean isLiteralConstant()
		{
			switch (arg.kind)
			{
				case EtchGrammarConstants.TRUE:
				case EtchGrammarConstants.FALSE:
				case EtchGrammarConstants.INTEGER:
				case EtchGrammarConstants.OCTAL:
				case EtchGrammarConstants.HEX:
				case EtchGrammarConstants.BINARY:
				case EtchGrammarConstants.DECIMAL:
				case EtchGrammarConstants.STR:
					return true;
				default:
					return false;
			}
		}
		
		/**
		 * @param msg
		 * @return true if this names a parameter of the protected
		 * method.
		 */
		public boolean isParameter( Message msg )
		{
			switch (arg.kind)
			{
				case EtchGrammarConstants.ID:
					return msg.hasParameter( arg.image );
				
				case EtchGrammarConstants.QID:
				{
					String[] n = StringUtil.leftSplit( arg.image, '.' );
					return msg.hasParameter( n[0] );
				}
				
				default:
					return false;
			}
		}
		
		/**
		 * @param intf
		 * @return true if this names a constant of the service.
		 */
		public boolean isConstant( Service intf )
		{
			if (arg.kind != EtchGrammarConstants.ID
					&& arg.kind != EtchGrammarConstants.QID)
				return false;
			
			Named<?> named = intf.get( arg.image );
			
			return named != null && named.isConstant();
		}
		
		/**
		 * @param intf
		 * @return true if this names an enum of the service.
		 */
		public boolean isEnum( Service intf )
		{
			if (arg.kind != EtchGrammarConstants.QID)
				return false;
			
			String[] n = StringUtil.leftSplit( arg.image, '.' );
			
			Named<?> named = intf.get( n[0] );
			
			return named != null && named.isEnumx();
		}
	}
}
