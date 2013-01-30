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

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.URL;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;

import org.apache.etch.compiler.ast.Message;
import org.apache.etch.compiler.ast.MessageDirection;
import org.apache.etch.compiler.ast.Module;
import org.apache.etch.compiler.ast.Name;
import org.apache.etch.compiler.ast.Named;
import org.apache.etch.compiler.ast.Opt;
import org.apache.etch.compiler.ast.ParamList;
import org.apache.etch.compiler.ast.Service;
import org.apache.etch.compiler.ast.TypeRef;
import org.apache.etch.compiler.opt.Extern;
import org.apache.etch.util.Assertion;


/**
 * Generic interface to language (or language/runtime) bindings.
 */
abstract public class Backend
{
	private final static String tmplPath1 = "org/apache/etch/compiler/";
	private final static String tmplPath2 = "resources/org/apache/etch/compiler/";
	
	//////////////////////////
	// WHAT DIRECTION GROUP //
	//////////////////////////
	
	/**
	 * Constant for option --what: generate both client and server files.
	 */
	public static final String WHAT_BOTH = "BOTH";
	
	/**
	 * Constant for option --what: generate client files.
	 */
	public static final String WHAT_CLIENT = "CLIENT";
	
	/**
	 * Constant for option --what: generate generate server files.
	 */
	public static final String WHAT_SERVER = "SERVER";

	//////////////////////
	// WHAT FILES GROUP //
	//////////////////////
	
	/**
	 * Constant for option --what: generate all files.
	 */
	public static final String WHAT_ALL = "ALL";
	
	/**
	 * Constant for option --what: generate no files.
	 */
	public static final String WHAT_NONE = "NONE";
	
	/**
	 * Constant for option --what: generate intf files.
	 */
	public static final String WHAT_INTF = "INTF";
	
	/**
	 * Constant for option --what: generate main file.
	 */
	public static final String WHAT_MAIN = "MAIN";
	
	/**
	 * Constant for option --what: generate impl files.
	 */
	public static final String WHAT_IMPL = "IMPL";

	/////////////////////
	// WHAT MISC GROUP //
	/////////////////////
	
	/**
	 * Constant for option --what: overwrite template files.
	 */
	public static final String WHAT_FORCE = "FORCE";
	
	/**
	 * Constant for option --what: give help on what.
	 */
	public static final String WHAT_HELP = "HELP";
	
	/**
	 * Constructs the Backend.
	 * @throws Exception
	 */
	protected Backend()
		throws Exception
	{
		String[] path = { tmplPath1, tmplPath2 };
		global_kwd = getPath(path, "globalKeywords.kwd");
	}

	/**
	 * Path to global keyword resource.
	 */
	protected final String global_kwd;

	@Override
	public String toString()
	{
		return getLang();
	}

	/**
	 * Generates code for this binding of the idl.
	 * @param module the module that we are generating code for.
	 * @param options the options used to configure the compiler.
	 * @throws Exception
	 */
	abstract public void generate( Module module, CmdLineOptions options )
		throws Exception;

	/**
	 * Runs the generator for a particular file with the specified
	 * output stream.
	 */
	public interface Gen
	{
		/**
		 * @param pw
		 * @throws Exception
		 */
		void run( PrintWriter pw ) throws Exception;
	}

	/**
	 * Helper method.  Return true if the md is SERVER.
	 * @param md
	 * @return true if MessageDirection is SERVER.
	 */
	public boolean isServer(MessageDirection md)
	{
		if (md == null)
			throw new NullPointerException( "md == null" );
		return md == MessageDirection.SERVER;
	}

	/**
	 * Helper method.  Return true if the md is CLIENT.
	 * @param md
	 * @return true if MessageDirection is CLIENT.
	 */
	public boolean isClient(MessageDirection md)
	{
		if (md == null)
			throw new NullPointerException( "md == null" );
		return md == MessageDirection.CLIENT;
	}

	/**
	 * Helper methods. Return true if the md is BOTH.
	 * @param md
	 * @return true if MessageDirection is BOTH.
	 */
	public boolean isBoth( MessageDirection md )
	{
		if ( md == null )
			throw new NullPointerException( "md == null" );
		return md == MessageDirection.BOTH;
	}

	/**
	 * Returns the correct path to a resource.
	 * @param path the paths to try for opening the resources
	 * @param fn the file name of the file to open
	 * @return the correct path to a resource
	 * @throws FileNotFoundException
	 */
	protected String getPath(String[] path, String fn)
		throws FileNotFoundException
	{
		for (String p: path)
		{
			if (p == null)
				continue;
			
			String s = p + fn;
//			System.out.println( "trying "+s );
			
			try
			{
				URL temp = getClass().getClassLoader().getResource(s);
				if (temp != null)
						return s;
			}
			catch ( Exception e )
			{
				// ignore
//				System.out.println( "ignoring "+e );
			}
		}
		throw new FileNotFoundException(fn);
	}

	/**
	 * Creates (or deletes) the specified file.
	 * @param dir
	 * @param fn
	 * @param gen
	 * @throws Exception
	 */
	protected void doFile( Output dir, String fn, LogHandler lh, Gen gen )
		throws Exception
	{
		PrintWriter pw = new PrintWriter( dir.newOutputStream( fn ) );
		try
		{
			gen.run( pw );
		}
		finally
		{
			pw.close();
		}
	}

	/**
	 * Augments the mapping from user defined reserved words to the reserved
	 * words used by the compiler as parsed from a reserved word list stored in
	 * a file located at the specified path.
	 * @param filePath the path to the file containing reserved word mapping
	 * @param wordMap
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	protected void mapWords(String filePath, Map<String, String> wordMap)
		throws FileNotFoundException, IOException
	{
		String commentMarker = "#";
		String valuePrefix = "x";
		String keyValueDelimeter = "= \t";

		// Open file for parsing or throw an FileNotFoundError if resources is
		// unavailable.
		BufferedReader fileReader = null;
		try
		{
			// Assume file is internal to project:
			// EG) org/apache/etch/compiler/globalKeywords
			InputStream input = getClass().getClassLoader().getResourceAsStream(filePath);

			// Otherwise it's external
			// EG) ../foo/bar/someKeywords
			if (null == input)
				input = new FileInputStream(filePath);

			//fileReader = new BufferedReader(new InputStreamReader(getClass().getResourceAsStream(filePath)));
			fileReader = new BufferedReader(new InputStreamReader(input));

			String currentLine = fileReader.readLine();
			while (null != currentLine)
			{
				// Strip trailing comments and padded whitespace
				currentLine = currentLine.trim();
				if (currentLine.contains(commentMarker))
					currentLine = currentLine.substring(0, currentLine.indexOf(commentMarker));

				// Skip blank lines
				StringTokenizer currentTokens = new StringTokenizer(currentLine, keyValueDelimeter);
				if (currentTokens.hasMoreTokens())
				{
					String key 	= currentTokens.nextToken();
					String value;

					if (currentTokens.hasMoreTokens())
						value = currentTokens.nextToken();
					else
						value = Character.isUpperCase(key.codePointAt(0)) ?
								valuePrefix.toUpperCase() + key :
								valuePrefix + key;

					wordMap.put(key, value);
				}

				currentLine = fileReader.readLine();
			}
		}

		catch (NullPointerException e)
		{
			throw new FileNotFoundException(filePath);
		}

		finally
		{
			if (null != fileReader)
				fileReader.close();
		}
	}

	/**
	 * @param what
	 * @return the appropriate direction give --what command line options.
	 */
	protected MessageDirection getMessageDirection(Set<String> what)
	{
		if ( what.contains( WHAT_BOTH )  )
			return MessageDirection.BOTH;
		
		if ( what.contains( WHAT_SERVER ) )
			return MessageDirection.SERVER;
		
		if ( what.contains( WHAT_CLIENT ) )
			return MessageDirection.CLIENT;

		throw new IllegalArgumentException( "what not correctly specified" );
	}
	
	/**
	 * @param what
	 * @return what modified to account for interaction of options.
	 */
	protected Set<String> populateWhat( Set<String> what ) throws Exception
	{
		checkOption( what );
		
		if (what.contains( WHAT_HELP ))
			throw new Exception(
				"specify what as one or more of "+whatHelp() );
		
		// direction group
		
		if (!what.contains( WHAT_CLIENT )
			&& !what.contains( WHAT_SERVER )
			&& !what.contains( WHAT_BOTH ) )
		{
			// default to BOTH if none of the direction group is present.
			what.add( WHAT_BOTH );
			what.add( WHAT_CLIENT );
			what.add( WHAT_SERVER );
		}
		else if (what.contains( WHAT_BOTH ))
		{
			// BOTH implies CLIENT and SERVER
			what.add( WHAT_CLIENT );
			what.add( WHAT_SERVER );
		}
		else if (what.contains( WHAT_CLIENT ) && what.contains( WHAT_SERVER ))
		{
			// CLIENT and SERVER together implies BOTH
			what.add( WHAT_BOTH );
		}
		
		// there are three reasonable outcomes of the above logic:
		// CLIENT
		// SERVER
		// BOTH, CLIENT, SERVER
		Assertion.check(
			   what.contains( WHAT_CLIENT ) && !what.contains( WHAT_SERVER ) && !what.contains( WHAT_BOTH )
			|| !what.contains( WHAT_CLIENT ) && what.contains( WHAT_SERVER ) && !what.contains( WHAT_BOTH )
			|| what.contains( WHAT_CLIENT ) && what.contains( WHAT_SERVER ) && what.contains( WHAT_BOTH )
			, "what files is {CLIENT}, {SERVER}, OR {CLIENT,SERVER,BOTH}" );
		
		// files group
		
		if (!what.contains( WHAT_ALL )
			&& !what.contains( WHAT_NONE )
			&& !what.contains( WHAT_INTF )
			&& !what.contains( WHAT_MAIN )
			&& !what.contains( WHAT_IMPL ))
		{
			// default to INTF if none of the file group is present.
			what.add( WHAT_INTF );
		}
		
		if (what.contains( WHAT_ALL ))
		{
			what.add( WHAT_INTF );
			what.add( WHAT_MAIN );
			what.add( WHAT_IMPL );
			what.remove( WHAT_ALL );
	        what.remove( WHAT_NONE );
		}
		
		if (what.contains( WHAT_NONE ))
		{
	        what.remove( WHAT_INTF );
	        what.remove( WHAT_MAIN );
	        what.remove( WHAT_IMPL );
	        what.remove( WHAT_NONE );
		}
		
		return what;
	}

	private void checkOption( Set<String> what ) throws Exception
	{
		Set<String> w = new HashSet<String>( what );
		
		// what direction group
		w.remove( WHAT_CLIENT );
		w.remove( WHAT_SERVER );
		w.remove( WHAT_BOTH );
		
		// what files group
		w.remove( WHAT_ALL );
		w.remove( WHAT_NONE );
		w.remove( WHAT_INTF );
		w.remove( WHAT_MAIN );
		w.remove( WHAT_IMPL );
		
		w.remove( WHAT_FORCE );
		w.remove( WHAT_HELP );
		
		if (!w.isEmpty())
			throw new Exception(
				"bad what option value(s): "+w
				+"; specify what as one or more of "
				+ whatHelp() );
	}
	
	private String whatHelp()
	{
		return WHAT_CLIENT
			+", "
			+WHAT_SERVER
			+", "
			+WHAT_BOTH
			+", "
			+WHAT_ALL
			+", "
			+WHAT_NONE
			+", "
			+WHAT_INTF
			+", "
			+WHAT_MAIN
			+", "
			+WHAT_IMPL
			+", "
			+WHAT_FORCE
			+", "
			+WHAT_HELP;
	}


	/**
	 * @param vname
	 * @return generic type vname into a binding appropriate
	 * vname.
	 */
	abstract public String mtvname( String vname );

	/**
	 * @param vname
	 * @return generic field vname into a binding appropriate
	 * vname.
	 */
	abstract public String mfvname( String vname );

	/**
	 * @return the lang being compiled.
	 */
	abstract public String getLang();

	/**
	 * @param msg a Message with the async receiver property.
	 * @return the name of the async receiver thread pool name to
	 * use for this async receiver method.
	 */
	abstract public String asyncReceiverPoolName( Message msg );

	/**
	 * @param type
	 * @param value
	 * @return the value properly formatted for the specified type.
	 * @throws IOException
	 * @throws Exception
	 */
	abstract public String getTypeValue( TypeRef type, Token value )
		throws IOException, Exception;

	/**
	 * @param n
	 * @param isExcept
	 * @return the properly formatted "toString()" format string.
	 * @throws ParseException
	 * @throws IOException
	 */
	abstract public String formatString( ParamList<Service> n,
		boolean isExcept ) throws ParseException, IOException;

	/**
	 * @param type
	 * @return type name appropriate for use as a structure
	 * element or exception parameter or function parameter
	 * or result.
	 */
	abstract public String getTypeName( TypeRef type );

	/**
	 * @param type the etch type
	 * @return the fundamental native type for java. so etch
	 * int -> java int, while etch string -> java String. this
	 * is used when typing constants that don't want ref types.
	 */
	abstract public String getNativeTypeName( TypeRef type );

	/**
	 * Returns the external fully qualified name for the enum. This might
	 * have to be modified over the normal fqname for the enum, which would
	 * be moduleName+'.'+serviceName+'.'+enumName.
	 * @param fqname
	 * @param moduleName
	 * @param serviceName
	 * @param enumName
	 * @return the external fqname.
	 */
	abstract public String enum_efqname( String fqname, String moduleName,
		String serviceName, String enumName );

	/**
	 * Returns the external fully qualified name for the enum. This might
	 * have to be modified over the normal fqname for the enum, which would
	 * be moduleName+'.'+serviceName+'.'+enumName.
	 * @param fqname
	 * @param moduleName
	 * @param serviceName
	 * @param enumName
	 * @return the external fqname.
	 */
	abstract public String struct_efqname( String fqname, String moduleName,
		String serviceName, String enumName );

	/**
	 * Returns the external fully qualified name for the enum. This might
	 * have to be modified over the normal fqname for the enum, which would
	 * be moduleName+'.'+serviceName+'.'+enumName.
	 * @param fqname
	 * @param moduleName
	 * @param serviceName
	 * @param enumName
	 * @return the external fqname.
	 */
	abstract public String except_efqname( String fqname, String moduleName,
		String serviceName, String enumName );

	/**
	 * @param intf the interface of the constant.
	 * @param name an id or qid which references a constant.
	 * @return the qualified external name.
	 */
	abstract public String qualifyConstantName( Service intf, Token name );

	/**
	 * @param intf the interface of the enum.
	 * @param name an id or qid which references an enum.
	 * @return the qualified external name.
	 */
	abstract public String qualifyEnumName( Service intf, Token name );

	/**
	 * @param name an id or qid which references a parameter of
	 * a method.
	 * @return the qualified parameter name.
	 */
	abstract public String qualifyParameterName( Token name );

	/**
	 * @param named
	 * @return the validator for the parameter.
	 */
	abstract public String getValidator( Named<?> named );

	/**
	 * Once a service is created, the binding may add default
	 * items to it such as standard types, externs, etc.
	 * @param service a newly created service, ready for
	 * defaults.
	 * @throws ParseException
	 */
	abstract public void addDefaults( Service service ) throws ParseException;

	/**
	 * Adds an extern declaration to the service.
	 * @param service the server to be modified.
	 * @param name the name of the type of the extern.
	 * @param language the language binding of this specification.
	 * @param xname the binding specific name of the type.
	 * @param xnameImport the "import" of the type if needed.
	 * @param serializer the binding specific name of the import/export helper.
	 * @param serializerImport the "import" of the import/export helper if needed.
	 * @throws ParseException
	 */
	protected void addExtern( Service service, Name name, Token language,
		Token xname, Token xnameImport, Token serializer,
		Token serializerImport ) throws ParseException
	{
		Map<String, Opt> opts = new HashMap<String, Opt>();
		opts.put( "Extern."+language.image, new Extern( name, language, xname,
			xnameImport, serializer, serializerImport ) );
		service.addExtern( name, opts );
	}

	/**
	 * Adds a built-in declaration to the service.
	 * @param service
	 * @param name
	 * @param bindingName
	 * @param allowSubclass
	 * @throws ParseException
	 */
	protected void addBuiltin( Service service, Name name, String bindingName,
		boolean allowSubclass ) throws ParseException
	{
		service.addBuiltin( name, bindingName, allowSubclass );
	}

	/**
	 * Constructs a name (with associated Id token).
	 * @param name
	 * @return the Name with ID token.
	 */
	protected Name newName( String name )
	{
		Token t = newId( name );
		return new Name( t, name );
	}
	
	/**
	 * Constructs an ID token.
	 * @param id
	 * @return the ID token.
	 */
	protected Token newId( String id )
	{
		return newToken( EtchGrammarConstants.ID, id );
	}
	
	/**
	 * Constructs a STR token.
	 * @param s
	 * @return the STR token.
	 */
	protected Token newString( String s )
	{
		return newToken( EtchGrammarConstants.STR, s );
	}
	
	/**
	 * Constructs a token of specified kind.
	 * @param kind
	 * @param image
	 * @return the token.
	 */
	protected Token newToken( int kind, String image )
	{
		Token t = new Token();
		t.kind = kind;
		t.image = image;
		return t;
	}
}
