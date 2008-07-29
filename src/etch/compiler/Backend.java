/* $Id$
 *
 * Created by wert on Apr 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.net.URL;
import java.util.HashSet;
import java.util.Map;
import java.util.Properties;
import java.util.Set;
import java.util.StringTokenizer;

import org.apache.velocity.Template;
import org.apache.velocity.app.Velocity;
import org.apache.velocity.exception.ResourceNotFoundException;

import etch.compiler.ast.Message;
import etch.compiler.ast.MessageDirection;
import etch.compiler.ast.Module;
import etch.compiler.ast.Named;
import etch.compiler.ast.ParamList;
import etch.compiler.ast.Service;
import etch.compiler.ast.TypeRef;

/**
 * Generic interface to language (or language/runtime) bindings.
 */
abstract public class Backend
{
	private final static String tmplPath1 = "etch/compiler/";
	private final static String tmplPath2 = "src/etch/compiler/";
	private final static String tmplPath3 = "";
	
	/**
	 * Constant for Command Line option BOTH
	 */
	public static final String WHAT_BOTH = "BOTH";
	/**
	 * Constant for Command Line option ALL
	 */
	public static final String WHAT_ALL = "ALL";
	/**
	 * Constant for Command Line option CLIENT
	 */
	public static final String WHAT_CLIENT = "CLIENT";
	/**
	 * Constant for Command Line option SERVER
	 */
	public static final String WHAT_SERVER = "SERVER";
	/**
	 * Constant for Command Line option IMPL
	 */
	public static final String WHAT_IMPL = "IMPL";
	/**
	 * Constant for Command Line option MAIN
	 */
	public static final String WHAT_MAIN = "MAIN";
	/**
	 * Constant for Command Line option HELPER
	 */
	public static final String WHAT_HELPER = "HELPER";
	/**
	 * Constant for Command Line option FORCE
	 */
	public static final String WHAT_FORCE = "FORCE";
	/**
	 * Constant for Command Line option NONE
	 */
	public static final String WHAT_NONE = "NONE";
	

	/**
	 * Constructs the Backend.
	 * @throws Exception 
	 */
	protected Backend()
		throws Exception
	{
		String[] path = { tmplPath1, tmplPath2, tmplPath3 };
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
	 * @param what the side we should generate (server, client, or both).
	 * @param dir the directory to place generated files. null means the
	 * @param ignoreGlobal the flag to ignore reserved words global to etch
	 * @param ignoreLocal the flag to ignore reserved words local to a binding
	 * @param userWords the path to a user defined reserved words list
	 * current directory.
	 * @throws Exception
	 */
	abstract public void generate( Module module, Set<String> what,
		File dir, boolean ignoreGlobal, boolean ignoreLocal, String userWords)
	throws Exception;

	/**
	 * Runs the generator for a particular file with the specified
	 * output stream. See
	 *
	 * @see #doFile(File, String, boolean, boolean, Gen)
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
		return (md == MessageDirection.SERVER);
	}

	/**
	 * Helper method.  Return true if the md is CLIENT.
	 * @param md
	 * @return true if MessageDirection is CLIENT.
	 */
	public boolean isClient(MessageDirection md)
	{
		return (md == MessageDirection.SERVER);
	}

	/**
	 * Initializes use of velocity engine and sets up
	 * resource loaders.
	 * @throws Exception
	 */
	protected void initVelocity()
		throws Exception
	{
		Properties props = new Properties();
		props.put( "resource.loader", "file, class" );

		props.put( "file.resource.loader.description", "Velocity File Resource Loader" );
		props.put( "file.resource.loader.class", "org.apache.velocity.runtime.resource.loader.FileResourceLoader" );
		props.put( "file.resource.loader.path", "." );

		props.put( "class.resource.loader.description", "Velocity Classpath Resource Loader" );
		props.put( "class.resource.loader.class", "org.apache.velocity.runtime.resource.loader.ClasspathResourceLoader" );

		Velocity.init( props );
	}

	/**
	 * @param path
	 * @param fn
	 * @return the velocity template
	 * @throws Exception
	 */
	protected Template getTemplate( String[] path, String fn )
		throws Exception
	{
		ResourceNotFoundException rnfe = null;
		for (String p: path)
		{
			try
			{
				return Velocity.getTemplate( p+fn );
			}
			catch ( ResourceNotFoundException e )
			{
				rnfe = e;
			}
		}
		throw rnfe;
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
		URL temp;
		for (String p: path)
		{
			temp = ClassLoader.getSystemResource(p + fn);
			if (temp != null)
					return p + fn;
		}
		throw new FileNotFoundException(fn);
	}

	/**
	 * Creates (or deletes) the specified file.
	 * @param dir
	 * @param fn
	 * @param makeFile
	 * @param force TODO
	 * @param gen
	 * @throws Exception
	 */
	protected void doFile( File dir, String fn, boolean makeFile, boolean force, Gen gen )
		throws Exception
	{
		File f = new File( dir, fn );

		if (makeFile)
		{
			if (!f.isFile())
				f.createNewFile();
			else if (!force)
			{
				System.err.printf( "file %s not generated because already exists; use -w force to overwrite\n", fn );
				return;
			}

			PrintWriter pw = new PrintWriter( new BufferedOutputStream( new FileOutputStream( f ) ) );
			try
			{
				gen.run( pw );
			}
			finally
			{
				pw.close();
			}
		}
		else if (f.isFile())
		{
			f.delete();
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
			// EG) etch/compiler/globalKeywords
			InputStream input = ClassLoader.getSystemResourceAsStream(filePath);

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
						value = (Character.isUpperCase(key.codePointAt(0))) ?
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
	protected MessageDirection getMessageDirection(Set<String> what) {

		if ( what.contains( WHAT_BOTH )  ) {
			return MessageDirection.BOTH;
		}
		if ( what.contains( WHAT_SERVER ) ) {
			return MessageDirection.SERVER;
		}
		if ( what.contains( WHAT_CLIENT ) ) {
			return MessageDirection.CLIENT;
		}

		return null; // default value
	}
	
	/**
	 * @param what
	 * @return what modified to account for all, both, and none options.
	 */
	protected Set<String> populateWhat(Set<String> what) {

		if(!checkOption(what))
		{
			return null;
		}
		
		if (what.contains( WHAT_ALL ))
		{
			what.add( WHAT_BOTH );
			what.add( WHAT_IMPL );
			what.add( WHAT_MAIN );
			what.add( WHAT_HELPER );
		}
		
		if (what.contains( WHAT_BOTH ))
		{
			what.add( WHAT_CLIENT );
			what.add( WHAT_SERVER );
		}

		if (what.contains( WHAT_NONE ))
		{
	        what.clear();
		}
		
//		if (what.isEmpty())
//		{
//			return what;
//		}
		
		return what;
	}

	private boolean checkOption( Set<String> what )
	{
		Set<String> w = new HashSet<String>( what );
		w.remove( WHAT_BOTH );
		w.remove( WHAT_CLIENT );
		w.remove( WHAT_SERVER );
		w.remove( WHAT_ALL );
		w.remove( WHAT_NONE );
		w.remove( WHAT_IMPL );
		w.remove( WHAT_MAIN );
		w.remove( WHAT_HELPER );
		w.remove( WHAT_FORCE );

		return w.isEmpty();
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
}
