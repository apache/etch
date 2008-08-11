/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.bindings.javascript.compiler;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringReader;
import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.Velocity;
import org.apache.velocity.exception.ResourceNotFoundException;
import org.apache.velocity.runtime.RuntimeServices;
import org.apache.velocity.runtime.log.LogChute;

import etch.compiler.Backend;
import etch.compiler.CmdLineOptions;
import etch.compiler.EtchGrammarConstants;
import etch.compiler.LogHandler;
import etch.compiler.ParseException;
import etch.compiler.Token;
import etch.compiler.Version;
import etch.compiler.Backend.Gen;
import etch.compiler.ast.Builtin;
import etch.compiler.ast.Except;
import etch.compiler.ast.Item;
import etch.compiler.ast.Message;
import etch.compiler.ast.MessageDirection;
import etch.compiler.ast.Module;
import etch.compiler.ast.MsgDirHelper;
import etch.compiler.ast.Name;
import etch.compiler.ast.Named;
import etch.compiler.ast.ParamList;
import etch.compiler.ast.Parameter;
import etch.compiler.ast.ReservedWordChecker;
import etch.compiler.ast.Service;
import etch.compiler.ast.Struct;
import etch.compiler.ast.Thrown;
import etch.compiler.ast.TypeRef;
import etch.compiler.opt.ToString;
import etch.compiler.opt.ToString.FieldItem;
import etch.compiler.opt.ToString.FmtItem;
import etch.compiler.opt.ToString.StringItem;
import etch.util.Assertion;

/**
 * Compiler is a helper class not only for Backend, but also for the templates.
 * They call methods here to perform "hard" tasks.
 */
public class Compiler extends Backend
{
	private final static String tmplPath1 = "etch/bindings/javascript/compiler/";

	private final static String tmplPath2 = "src/etch/bindings/javascript/compiler/";

	private final static String fnSuffix = ".js";

	private final static String VERSION = Version.VERSION + " / "
		+ CompilerVersion.VERSION;

	/**
	 * Constructs the Compiler. This is a helper class not only for Backend, but
	 * also for the templates. They call methods here to perform "hard" tasks.
	 *
	 * @throws Exception
	 */
	public Compiler() throws Exception
	{
		initVelocity();
		String[] path =
		{
			tmplPath1, tmplPath2
		};
		lib = getTemplate( path, "lib.vm" );

		local_kwd = getPath( path, "javascriptKeywords.kwd" );
	}

	private final Template lib;

	private final String local_kwd;

	private LogHandler lh;
	
	private Service topInterface = null;

	
	/**
	 * Initializes use of velocity engine and sets up
	 * resource loaders.
	 * @throws Exception
	 */
	private void initVelocity()
		throws Exception
	{
		Velocity.setProperty( Velocity.RUNTIME_LOG_LOGSYSTEM, new MyLogger() );
		
		Velocity.setProperty( Velocity.RESOURCE_LOADER, "file, class" );

		Velocity.setProperty( "file.resource.loader.description", "Velocity File Resource Loader" );
		Velocity.setProperty( "file.resource.loader.class", "org.apache.velocity.runtime.resource.loader.FileResourceLoader" );
		Velocity.setProperty( "file.resource.loader.path", "." );

		Velocity.setProperty( "class.resource.loader.description", "Velocity Classpath Resource Loader" );
		Velocity.setProperty( "class.resource.loader.class", "org.apache.velocity.runtime.resource.loader.ClasspathResourceLoader" );
		
		Velocity.init();
	}

	private static class MyLogger implements LogChute
	{
		private final LogHandler lh = null;
		
		public void init( RuntimeServices rts ) throws Exception
		{
			// ignore.
		}

		public boolean isLevelEnabled( int level )
		{
			return level >= 2;
		}

		public void log( int level, String msg )
		{
			if (level < 2)
				return;
			
			if (lh != null)
				lh.logMessage( level == 2 ? LogHandler.LEVEL_WARNING : LogHandler.LEVEL_ERROR, null, msg );
			else
				System.out.printf( "Velocity msg (%d): %s\n", level, msg );
		}

		public void log( int level, String msg, Throwable e )
		{
			if (level < 2)
				return;
			
			if (lh != null)
				lh.logMessage( level == 2 ? LogHandler.LEVEL_WARNING : LogHandler.LEVEL_ERROR, null, msg );
			else
				System.out.printf( "Velocity msg (%d): %s: %s\n", level, msg, e );
		}
	}

	/**
	 * @param path
	 * @param fn
	 * @return the velocity template
	 * @throws Exception
	 */
	private Template getTemplate( String[] path, String fn )
		throws Exception
	{
		ResourceNotFoundException rnfe = null;
		
		for (String p: path)
		{
			if (p == null)
				continue;
			
//			System.out.println( "trying to load template "+(p+fn) );
			try
			{
				if (Velocity.resourceExists( p+fn ))
					return Velocity.getTemplate( p+fn );
			}
			catch ( ResourceNotFoundException e )
			{
				rnfe = e;
			}
			catch ( Exception e )
			{
				System.out.println( "ignoring "+e);
			}
		}
		
		if (rnfe != null)
			throw rnfe;
		
		throw new ResourceNotFoundException("could not find resource: "+fn);
	}

	@Override
	public void generate( Module module, CmdLineOptions options, LogHandler _lh )
		throws Exception
	{
		this.lh = _lh;

		boolean ignoreGlobal = options.ignoreGlobal;
		boolean ignoreLocal = options.ignoreLocal;
		String userWords = options.userWordsList;
		File dir = options.outputDir;
		Set<String> what = options.what;

		// Load the reserved words lists if any have been provided.
		Map<String, String> words = new HashMap<String, String>();
		if (!ignoreGlobal)
			mapWords( global_kwd, words );
		if (!ignoreLocal)
			mapWords( local_kwd, words );
		if (userWords != null)
			mapWords( userWords, words );

		// check for collisions with the reserved word list.
		ReservedWordChecker checker = new ReservedWordChecker( words, false,_lh );
		module.treewalk( checker );
		if (!checker.ok())
		{
			_lh.logMessage( LogHandler.LEVEL_ERROR, null, "Encountered errors during java generation.\n" );
			return;
		}

		// ok, we're ready to generate code. make sure the
		// output directories exist.

		if (dir != null && module.name().name.length() > 0)
		{
			String path = !options.noFlattenPackages
				? module.name().name
				: module.name().name.replace( '.', '/' );
			dir = new File( dir, path );
		}
		else
		{
			dir = new File( "." );
		}

		dir.mkdirs();

		// generate code for each service.

		for (Service intf : module)
		{
			if(topInterface == null)
			{
				// capture the first service, which is assumed to be the topmost
				topInterface = intf;
			}
			gIntf = intf;
			generate( intf, what, dir, topInterface);
		}
		gIntf = null;
	}

	private Service gIntf;

	private void generate( final Service intf, Set<String> what, File dir, final Service topInterface )
		throws Exception
	{
		what = populateWhat( what );

		if (what == null)
		{
			// lh.logMessage( lh.LEVEL_ERROR, null,
			// "User has selected invalid option\n" );
			// return;
			throw new Exception(
				"User has selected invalid option. Valid Options are"
					+ " all,both,server,client,impl,main,helper,none\n" );
		}

		if (what.isEmpty())
		{
			// lh.logMessage( lh.LEVEL_ERROR, null, "User has selected NONE\n" );
			return;
		}

		final MessageDirection msgDir = getMessageDirection( what );

		if (what.contains( WHAT_CLIENT ))		
		{
			generateImpl( intf, dir, msgDir, intf == topInterface, topInterface);
		}
	}

	/**
	 * Generate the example user implemention class which extends the base
	 * implementation generated above. This class will only have the appropriate
	 * constructor and reference to the appropriate remote, and a comment inviting
	 * the user to override methods.
	 * @param pw
	 * @param intf
	 * @param mc
	 * @param hasBaseClass
	 * @throws Exception
	 */
	private void generateImpl(final Service intf, File dir, final MessageDirection mc, final boolean isTopInterface, final Service topInterface ) throws Exception
	{
		doFile( dir, this.getImplName(intf, mc)
				+ fnSuffix, true, true, new Gen()
			{
				public void run( PrintWriter pw ) throws Exception
				{
					generateImpl(pw, intf, mc, isTopInterface, topInterface);
				}
			}, lh );

	}
	
	void generateImpl(PrintWriter pw, Service intf, MessageDirection mc, boolean isTopInterface, Service topInterface) throws Exception
	{
		VelocityContext context = new VelocityContext();
		context.put( "now", new Date() );
		context.put( "version", VERSION );
		context.put( "helper", this );
		context.put( "intf", intf );
		context.put( "mc", mc );
		context.put( "suffix", MsgDirHelper.getSuffix( mc ) );
		context.put( "isTopInterface", isTopInterface);
		context.put( "topIntf", topInterface); 
		lib.merge( context, pw );
	}

	private String getImplName( Service intf, MessageDirection mc )
	{
		return intf.fqname();
	}

	@Override
	public String asyncReceiverPoolName( Message msg )
	{
		return msg.getAsyncReceiver().toString().toLowerCase();
	}

	@Override
	public String getTypeValue( TypeRef type, Token value )
	{
		// System.out.println( "getTypeValue called with: "+type+": "+value );
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.LONG:
				return value.image + "L";
			case EtchGrammarConstants.FLOAT:
				return value.image + "F";
			case EtchGrammarConstants.DOUBLE:
				return value.image + "D";
			case EtchGrammarConstants.STRING:
				return protectString( value.image );
			default:
				return value.image;
		}
	}

	private String protectString( String s )
	{
		// System.out.println( "protectString called with: "+s );

		StringBuffer sb = new StringBuffer();
		sb.append( "\"" );
		for (char c : s.toCharArray())
		{
			if (c == '\t')
			{
				sb.append( "\\t" );
				continue;
			}
			if (c == '\r')
			{
				sb.append( "\\r" );
				continue;
			}
			if (c == '\n')
			{
				sb.append( "\\n" );
				continue;
			}
			if (c == '\"')
			{
				sb.append( "\\\"" );
				continue;
			}
			if (c == '\\')
			{
				sb.append( "\\\\" );
				continue;
			}
			if (c >= 32 && c < 127)
			{
				sb.append( c );
				continue;
			}
			sb.append( String.format( "\\u%04x", (int) c ) );
		}
		sb.append( "\"" );
		return sb.toString();
	}
	
	public String newline()
	{
		return "\r\n";
	}

	/**
	 * @param type
	 * @return type name appropriate for use as a structure element or exception
	 * parameter or function parameter or result.
	 */
	@Override
	public String getTypeName( TypeRef type )
	{
		if (type.dim() > 0)
			return getNativeTypeName( type ) + dim2spec( type.dim() );
		return getRefTypeName( type );
	}

	/**
	 * @param type the etch type
	 * @return the fundamental native type for java. so etch int -> csharp int,
	 * while etch string -> csharp string.
	 */
	@Override
	public String getNativeTypeName( TypeRef type )
	{
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.VOID:
				return "";
			case EtchGrammarConstants.BOOLEAN:
				return "";
			case EtchGrammarConstants.BYTE:
				return "";
			case EtchGrammarConstants.SHORT:
				return "";
			case EtchGrammarConstants.INT:
				return "";
			case EtchGrammarConstants.LONG:
				return "";
			case EtchGrammarConstants.FLOAT:
				return "";
			case EtchGrammarConstants.DOUBLE:
				return "";
			case EtchGrammarConstants.STRING:
				return "";
			case EtchGrammarConstants.OBJECT:
				return "";
			default:
			{
				// we have to use a fully qualified name here.
				// find the actual type...
				Named<?> n = gIntf.get( t.image );
				return n.efqname( this );
			}
		}
	}

	/**
	 * @param type the etch type
	 * @return the fundamental native reference type for java. so etch int ->
	 * csharp int?, while etch string -> csharp string.
	 */
	private String getRefTypeName( TypeRef type )
	{
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.VOID:
				return "";
			case EtchGrammarConstants.BOOLEAN:
				return "";
			case EtchGrammarConstants.BYTE:
				return "";
			case EtchGrammarConstants.SHORT:
				return "";
			case EtchGrammarConstants.INT:
				return "";
			case EtchGrammarConstants.LONG:
				return "";
			case EtchGrammarConstants.FLOAT:
				return "";
			case EtchGrammarConstants.DOUBLE:
				return "";
			case EtchGrammarConstants.STRING:
				return "";
			case EtchGrammarConstants.OBJECT:
				return "";
			default:
			{
				// we have to use a fully qualified name here.
				// find the actual type...
				Named<?> n = gIntf.get( t.image );
				if (n.isEnumx())
					return (n.efqname( this ) + "?");
				return n.efqname( this );
			}
		}
	}

	private String dim2spec( int i )
	{
		String s = "";
		while (i-- > 0)
			s += "[]";
		return s;
	}



	private void escape( StringBuffer sb, String s ) throws IOException
	{
		StringReader rdr = new StringReader( s );
		int c;
		while ((c = rdr.read()) >= 0)
		{
			if (c == '"')
				sb.append( "\\\"" );
			else if (c == '\\')
				sb.append( "\\\\" );
			else if (c == '\t')
				sb.append( "\\t" );
			else if (c == '\r')
				sb.append( "\\r" );
			else if (c == '\n')
				sb.append( "\\n" );
			else
				sb.append( (char) c );
		}
	}
	
	@Override
	public String getLang()
	{
		return "javascript";
	}
	
	public String prototype()
	{
		return ".prototype.";
	}
	
	public String dot()
	{
		return ".";
	}
	
	public boolean isServerDirected(MessageDirection messageDirection)
	{
		return messageDirection == MessageDirection.BOTH || messageDirection == MessageDirection.SERVER;
	}
	
	public boolean isClientDirected(MessageDirection messageDirection)
	{
		return messageDirection == MessageDirection.BOTH || messageDirection == MessageDirection.CLIENT;
	}
	
	
	public String topInterfaceName()
	{
		return topInterface.name().toString();
	}
	/*
	 * Used to create a locally-correct struct name with the prototype field qualifying it,
	 * i.e.,   Test.A, where Test = service name, A = struct name
	 */
	public String localJsStructName(String serviceName, String structName)
	{
		return serviceName + "." + structName;
	}
	
	/*
	 * Used to create a locally-correct struct name with the prototype field qualifying it,
	 * i.e.,   Test.A.prototype, where Test = service name, A = struct name
	 */
	public String localStructPrototype(String serviceName, String structName)
	{
		return localJsStructName(serviceName, structName) + ".prototype";
	}

	/*
	 * Used to create a locally-correct struct name with the prototype.constructor field qualifying it,
	 * i.e.,   Test.A.prototype.constructor, where Test = service name, A = struct name
	 */
	public String localStructConstructor(String serviceName, String structName)
	{
		return localJsStructName(serviceName, structName) + ".prototype.constructor";
	}
	
	/*
	 * Used to create a locally-correct struct name with the prototype.field field qualifying it,
	 * i.e.,   Test.A.prototype.field, where Test = service name, A = struct name, field = field name
	 */
	public String localStructProtoField(String serviceName, String structName, String fieldName)
	{
		return localJsStructName(serviceName, structName) + ".prototype." + fieldName;
	}
	

	@Override
	public String qualifyParameterName( Token name )
	{
		return name.image;
	}
	
	@Override
	public void addDefaults(Service service) throws ParseException {
		addBuiltin( service, newName( "List" ), "[]", true );
		addBuiltin( service, newName( "Map" ), "{}", true );
		addBuiltin( service, newName( "Set" ), "[]", true );
		addBuiltin( service, newName( "Datetime" ), "", false );
	}


	@Override
	public String enum_efqname(String fqname, String moduleName,
			String serviceName, String enumName) {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public String except_efqname(String fqname, String moduleName,
			String serviceName, String enumName) {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public String getValidator(Named<?> named) {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public String mfvname(String vname) {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public String mtvname(String vname) {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public String qualifyConstantName(Service intf, Token name) {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public String qualifyEnumName(Service intf, Token name) {
		// TODO Auto-generated method stub
		return null;
	}


	@Override
	public String struct_efqname(String fqname, String moduleName,
			String serviceName, String enumName) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String formatString(ParamList<Service> n, boolean isExcept)
			throws ParseException, IOException {
		// TODO Auto-generated method stub
		return null;
	}
}
