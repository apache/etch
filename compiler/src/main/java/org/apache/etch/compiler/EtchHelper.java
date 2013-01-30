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

package org.apache.etch.compiler;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.StringReader;
import java.io.UnsupportedEncodingException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import org.apache.etch.compiler.ast.Constant;
import org.apache.etch.compiler.ast.Enumx;
import org.apache.etch.compiler.ast.Except;
import org.apache.etch.compiler.ast.Item;
import org.apache.etch.compiler.ast.Message;
import org.apache.etch.compiler.ast.Module;
import org.apache.etch.compiler.ast.Name;
import org.apache.etch.compiler.ast.Opt;
import org.apache.etch.compiler.ast.ParamList;
import org.apache.etch.compiler.ast.Parameter;
import org.apache.etch.compiler.ast.Service;
import org.apache.etch.compiler.ast.Struct;
import org.apache.etch.compiler.ast.Thrown;


/**
 * Compiler support routines.
 */
public class EtchHelper
{
//	/** TODO static global variable lh in compiler needs to go away. */
//	public static LogHandler lh;
	
//	/** TODO static global variable cl in compiler needs to go away. */
//	public static CmdLineOptions cl;

	/**
	 * Parses and saves a module comment.
	 * @param m
	 * @param specialToken
	 */
	protected static void moduleComment( Module m, Token specialToken )
	{
		if (specialToken == null)
			return;

		List<String> list = deComment( specialToken.image );
		if (list == null)
			return;

		m.setDescr( list );
	}

	/**
	 * Parses and saves a service comment.
	 * @param s
	 * @param specialToken
	 */
	protected static void serviceComment( Service s, Token specialToken )
	{
		if (specialToken == null)
			return;

		List<String> list = deComment( specialToken.image );
		if (list == null)
			return;

		s.setDescr( list );
	}

	/**
	 * Processes the formal comment for a constant.
	 * @param c
	 * @param specialToken
	 */
	protected static void constantComment( Constant c, Token specialToken )
	{
		if (specialToken == null)
			return;

		List<String> list = deComment( specialToken.image );
		if (list == null)
			return;

		c.setDescr( list );
	}

	/**
	 * Processes the formal comment for an enum.
	 * @param e
	 * @param specialToken
	 */
	protected static void enumComment( Enumx e, Token specialToken )
	{
		if (specialToken == null)
			return;

		List<String> list = deComment( specialToken.image );
		if (list == null)
			return;

		// TODO if the items are described in @item tags here
		// add that to the appropriate item. This will look
		// alot like paramListComment.

		e.setDescr( list );
	}

	/**
	 * Processes the formal comment for an enum item.
	 * @param i
	 * @param specialToken
	 */
	protected static void itemComment( Item i, Token specialToken )
	{
		if (specialToken == null)
			return;

		List<String> list = deComment( specialToken.image );
		if (list == null)
			return;

		i.setDescr( list );
	}

	/**
	 * Processes the formal comment for a parameter of a struct, exception,
	 * or message.
	 * @param p
	 * @param specialToken
	 */
	protected static void parameterComment( Parameter p, Token specialToken )
	{
		if (specialToken == null)
			return;

		List<String> list = deComment( specialToken.image );
		if (list == null)
			return;

		p.setDescr( list );
	}

	/**
	 * Processes the formal comment for a thrown exception.
	 * @param t
	 * @param specialToken
	 */
	protected static void thrownComment( Thrown t, Token specialToken )
	{
		if (specialToken == null)
			return;

		List<String> list = deComment( specialToken.image );
		if (list == null)
			return;

		t.setDescr( list );
	}

	/**
	 * Processes the formal comment for a struct.
	 * @param s
	 * @param specialToken
	 */
	protected static void structComment( Struct s, Token specialToken )
	{
		paramListComment( s, specialToken, false );
	}

	/**
	 * Processes the formal comment for an exception.
	 * @param e
	 * @param specialToken
	 */
	protected static void exceptComment( Except e, Token specialToken )
	{
		paramListComment( e, specialToken, false );
	}

	/**
	 * Processes the formal comment for a message.
	 * @param m
	 * @param specialToken
	 */
	protected static void messageComment( Message m, Token specialToken )
	{
		paramListComment( m, specialToken, true );
	}

	/**
	 * @param t
	 * @throws IOException
	 */
	protected static void fixString( Token t ) throws ParseException
	{
		if (t.kind != EtchGrammarConstants.STR)
			return;

		try
		{
			t.image = unescapeStr( t.image );
		}
		catch ( IOException e)
		{
			throw new ParseException( String.format(
				"Problem with string '%s' at line %d", e.getMessage(), t.beginLine ) );
		}
	}

	private static String unescapeStr( String s ) throws IOException
	{
		// remove the quotes.

		if (!s.startsWith( "\"" ) || !s.endsWith( "\"" ))
			throw new IllegalArgumentException( "string image is not quoted" );

		s = s.substring( 1, s.length()-1 );

		StringReader rdr = new StringReader( s );
		StringBuffer sb = new StringBuffer();

		int c;
		while ((c = rdr.read()) >= 0)
		{
			if (c != '\\')
			{
				sb.append( (char) c );
				continue;
			}

			// c == '\\': two escape formats: uxxxx and k (k=["\trn])

			c = rdr.read();
			if (c == 'u')
			{
				// unicode escape (need for hex chars)
				c = fromHex( rdr.read() );
//				 System.out.println( "c1: "+c );
				c = (c << 4) + fromHex( rdr.read() );
//				 System.out.println( "c2: "+c );
				c = (c << 4) + fromHex( rdr.read() );
//				 System.out.println( "c3: "+c );
				c = (c << 4) + fromHex( rdr.read() );
//				 System.out.println( "c4: "+c );
				sb.append( (char) c );
			}
			else switch (c)
			{
				// plain escape
				case '"': sb.append( "\"" ); break;
				case '\\': sb.append( "\\" ); break;
				case 't': sb.append( "\t" ); break;
				case 'r': sb.append( "\r" ); break;
				case 'n': sb.append( "\n" ); break;
				default: throw new IOException( "bad escape char: "+c );
			}
		}
		return sb.toString();
	}

	private static int fromHex( int c ) throws IOException
	{
		if (c >= '0' && c <= '9') return c - '0';
		if (c >= 'a' && c <= 'f') return c - 'a' + 10;
		if (c >= 'A' && c <= 'F') return c - 'A' + 10;
		throw new IOException( "bad hex char: "+c );
	}

	/**
	 * Formal comment types (tags).
	 */
	enum Type
	{
		/**
		 * The main body of the comment before any tags.
		 */
		Main,
		/**
		 * The type denoting an @param tag.
		 */
		Param,
		/**
		 * The type denoting an @return tag.
		 */
		Return,
		/**
		 * The type denoting an @throws tag.
		 */
		Throws,
		/**
		 * The type denoting an unrecognized tag.
		 */
		None
	}

	/**
	 * Processes any saved message comment.
	 * @param m
	 */
	private static void paramListComment( ParamList<Service> m,
		Token specialToken, boolean isMessage )
	{
		if (specialToken == null)
			return;

		List<String> list = deComment( specialToken.image );
		if (list == null)
			return;

//		System.out.println( list );

		Type type = Type.Main;
		String name = null;
		for (String s: list)
		{
			String x = s;

//			System.out.printf( "processing: %s\n", s );
			if (s.startsWith( "@" ))
			{
				StringTokenizer st = new StringTokenizer( s, " \t" );
				String k = st.nextToken();
				if (k.equals( "@param" ))
				{
					type = Type.Param;
					if (!st.hasMoreTokens())
					{
						System.out.printf( "*** warning: parameter name missing on @param at line %d\n", specialToken.beginLine );
						return;
					}
					name = st.nextToken();
					if (st.hasMoreTokens())
						s = st.nextToken( "\r\n" ).trim();
					else
						s = "";
				}
				else if (k.equals( "@return" ))
				{
					type = Type.Return;
					if (!isMessage || !((Message) m).hasReturn())
					{
						System.out.printf( "*** warning: @return used on non-message or void message at line %d\n", specialToken.beginLine );
						return;
					}
					if (st.hasMoreTokens())
						s = st.nextToken( "\r\n" ).trim();
					else
						s = "";
				}
				else if (k.equals( "@throws" ))
				{
					type = Type.Throws;
					if (!isMessage)
					{
						System.out.printf( "*** warning: @throws used on non-message at line %d\n", specialToken.beginLine );
						return;
					}
					if (!st.hasMoreTokens())
					{
						System.out.printf( "*** warning: exception name missing on @throws at line %d\n", specialToken.beginLine );
						return;
					}
					name = st.nextToken();
					if (st.hasMoreTokens())
						s = st.nextToken( "\r\n" ).trim();
					else
						s = "";
				}
				else
				{
					type = Type.None;
					System.out.printf( "*** warning: %s not understood in this context (%s): %s\n", k, m.name(), x );
				}
			}

//			System.out.printf( "type %d\n", type );

			if (type == Type.Main)
			{
				//System.out.printf( "adding to m.descr: %s\n", s );
				m.descr().add( s );
			}
			else if (type == Type.Param)
			{
//				System.out.printf( "adding to p.descr (%s): %s\n", name, s );
				Parameter p = m.getParameter( name );
				if (p != null)
					p.descr().add( s );
				else
					System.out.printf( "*** warning: cannot document parameter %s of %s\n", name, m.name() );
			}
			else if (type == Type.Return)
			{
//				System.out.printf( "adding to m.returnDescr: %s\n", s );
				((Message) m).returnDescr().add( s );
			}
			else if (type == Type.Throws)
			{
//				System.out.printf( "adding to t.descr (%s): %s\n", name, s );
				Thrown t = ((Message) m).getThrown( name );
				if (t != null)
					t.descr().add( s );
				else
					System.out.printf( "*** warning: cannot document thrown %s of %s\n", name, m.name() );
			}
			else if (type != Type.None)
			{
				System.out.printf( "*** warning: comment type %s not understood\n", type );
			}
		}
	}

	/**
	 * Splits a formal comment into its lines, stripping
	 * off the comment gunk.
	 * @param s
	 * @return an array of lines of the comment, without
	 * the EOL characters and the comment gunk.
	 */
	protected static List<String> deComment( String s )
	{
		if (!s.startsWith( "/** " ) &&
				!s.startsWith( "/**\t" ) &&
				!s.startsWith( "/**\r" ) &&
				!s.startsWith( "/**\n" ))
			return null;

		s = s.substring( 3 );

		if (s.endsWith( "*/" ))
			s = s.substring( 0, s.length() - 2 );

		s = s.trim();

		// now we have a bunch of lines that look like this:
		// .*(\r\n\w[*](\W.*)?)*
		// undo all that by chopping up into lines, then removing
		// leading white space up to the * and then the space directly
		// after the * if there is one.

		List<String> list = new ArrayList<String>();

		StringTokenizer st = new StringTokenizer( s, "\r\n", true );
		while (st.hasMoreTokens())
		{
			String t = st.nextToken();
			if (t.equals( "\r" ))
			{
				// ignore the EOL marks.
			}
			else if (t.equals( "\n" ))
			{
				// ignore the EOL marks.
			}
			else
			// not an EOL marker...
			{
				t = t.trim();
				// this is a line that looks like "*blah" or
				// "* blah" or "*".
				if (t.startsWith( "* " ))
					list.add( t.substring( 2 ) );
				else if (t.startsWith( "*" ))
					list.add( t.substring( 1 ) );
				else
					list.add( t );
			}
		}
		return list;
	}

	/**
	 * @return the last saved comment.
	 */
	protected Token getComment()
	{
		Token comment = savedComment;
		savedComment = null;
		return comment;
	}

	private Token savedComment;

	/**
	 * Saves a comment to be used later. Only the
	 * most recently saved comment will be available.
	 * @param comment
	 */
	protected void saveComment( Token comment )
	{
		if (comment != null)
			savedComment = comment;
	}

	/**
	 * @param n the name of the opt.
	 * @param list the list of opt arguments.
	 * @return an Opt subclass
	 * @throws ParseException
	 */
	protected Opt makeOpt( Name n, List<Token> list ) throws ParseException
	{
		String cn = "org.apache.etch.compiler.opt."+n.token.image;
		Class<?> c;

		try
		{
			c = Class.forName( cn );
		}
		catch ( NoClassDefFoundError e )
		{
			throw new ParseException( String.format(
				"option class %s not found at line %d",
				n, n.token.beginLine ) );
		}
		catch ( ClassNotFoundException e )
		{
			throw new ParseException( String.format(
				"option class %s not found at line %d",
				n, n.token.beginLine ) );
		}

		Class<?>[] params = { Name.class, Token[].class };

		Constructor<?> k;

		try
		{
			k = c.getConstructor( params );
		}
		catch ( SecurityException e )
		{
			throw new ParseException( String.format(
				"security rules block access to the constructor of option class %s at line %d",
				n, n.token.beginLine ) );
		}
		catch ( NoSuchMethodException e )
		{
			throw new ParseException( String.format(
				"missing constructor for option class %s at line %d",
				n, n.token.beginLine ) );
		}

		Object[] args = { n, list.toArray( new Token[list.size()] ) };

		try
		{
			return (Opt) k.newInstance( args );
		}
		catch ( IllegalArgumentException e )
		{
			throw new ParseException( String.format(
				"illegal argument to newInstance for option class %s at line %d: %s",
				n, n.token.beginLine, e ) );
		}
		catch ( InstantiationException e )
		{
			throw new ParseException( String.format(
				"cannot instantiate option class %s at line %d: %s",
				n, n.token.beginLine, e ) );
		}
		catch ( IllegalAccessException e )
		{
			throw new ParseException( String.format(
				"illegal access of constructor for option class %s at line %d: %s",
				n, n.token.beginLine, e ) );
		}
		catch ( InvocationTargetException e )
		{
			if (e.getTargetException() instanceof ParseException)
				throw (ParseException) e.getTargetException();

			throw new ParseException( String.format(
				"exception caught from constructor for option class %s at line %d: %s",
				n, n.token.beginLine, e.getTargetException() ) );
		}
	}

	protected void doStartService()
	{
	}

	protected void doEndService(Service oServ)
	{
	}

	/**
	 * @param f name of the filename for the include.
	 */
	protected void doInclude( Token fileName, Service servobj) throws ParseException
	{

		//Get the Gramar Obj
		EtchGrammar oGramar = (EtchGrammar) this;

		servobj.getCmdLineOptions().lh.push( fileName.toString(), fileName.beginLine );

		//System.out.println("------Start Do Include-------");

		//Get the file name
	  	String oStr = fileName.toString().replaceAll("\"", "");

	 // 	System.out.println("Including " + oStr);
	  	servobj.getCmdLineOptions().lh.report( LogHandler.LEVEL_INFO, "Including %s", oStr );

	    //Create the input string and read it in
		InputStream oStream = null;

		// Search the etch path for the file
		for (File f : servobj.getCmdLineOptions().effectiveIncludePath)
		{
			try
			{
				oStream = new java.io.FileInputStream(new File(f, oStr));
				break;
			}
			catch (FileNotFoundException e)
			{
			}
		}

		if (oStream == null)
			throw new ParseException(" Included File " + oStr + " Not Found\n");

		//Keep for debugging
		//StackInputStream oStStream = new StackInputStream(oStream);
		SimpleCharStream curStream = null;
		try
		{
			curStream = new SimpleCharStream(oStream, null, 1, 1);
		}

		catch ( UnsupportedEncodingException e )
		{
			throw new ParseException(" Included File " + oStr + " is not correctly encoded\n");

		}

		//Create a new token manager with the new include stream
	    EtchGrammarTokenManager oTM = new EtchGrammarTokenManager(curStream);

	    //Keep the old token manager
	    EtchGrammarTokenManager oOldTM = oGramar.token_source;

	    //Keep the old token
	    Token token = oGramar.token;

	    //reinit with the new token manager
	    oGramar.ReInit( oTM );

	    	//Proc the stream
		oGramar.stmts( servobj );


		//Put the old data back
		oGramar.ReInit( oOldTM );
		oGramar.token = token;
		servobj.getCmdLineOptions().lh.pop( fileName.toString() );
		//System.out.println("------End Do Include---------");
	}
}
