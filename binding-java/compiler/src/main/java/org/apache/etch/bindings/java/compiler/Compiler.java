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

package org.apache.etch.bindings.java.compiler;

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

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.CmdLineOptions;
import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.LogHandler;
import org.apache.etch.compiler.Output;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.Token;
import org.apache.etch.compiler.Version;
import org.apache.etch.compiler.Backend.Gen;
import org.apache.etch.compiler.ast.Builtin;
import org.apache.etch.compiler.ast.Except;
import org.apache.etch.compiler.ast.Item;
import org.apache.etch.compiler.ast.Message;
import org.apache.etch.compiler.ast.MessageDirection;
import org.apache.etch.compiler.ast.Module;
import org.apache.etch.compiler.ast.MsgDirHelper;
import org.apache.etch.compiler.ast.Name;
import org.apache.etch.compiler.ast.Named;
import org.apache.etch.compiler.ast.ParamList;
import org.apache.etch.compiler.ast.Parameter;
import org.apache.etch.compiler.ast.ReservedWordChecker;
import org.apache.etch.compiler.ast.Service;
import org.apache.etch.compiler.ast.Struct;
import org.apache.etch.compiler.ast.Thrown;
import org.apache.etch.compiler.ast.TypeRef;
import org.apache.etch.compiler.opt.ToString;
import org.apache.etch.compiler.opt.ToString.FieldItem;
import org.apache.etch.compiler.opt.ToString.FmtItem;
import org.apache.etch.compiler.opt.ToString.StringItem;
import org.apache.etch.util.Assertion;
import org.apache.etch.util.Hash;
import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;
import org.apache.velocity.app.Velocity;
import org.apache.velocity.exception.ResourceNotFoundException;
import org.apache.velocity.runtime.RuntimeServices;
import org.apache.velocity.runtime.log.LogChute;


/**
 * Compiler is a helper class not only for Backend, but also for the templates.
 * They call methods here to perform "hard" tasks.
 */
public class Compiler extends Backend
{

	private final static String tmplPath1 = "org/apache/etch/bindings/java/compiler/";

	private final static String tmplPath2 = "resources/org/apache/etch/bindings/java/compiler/";

	private final static String fnSuffix = ".java";

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
		
		String[] path = { tmplPath1, tmplPath2 };
		vf_vm = getTemplate( path, "vf.vm" );
		intf_vm = getTemplate( path, "intf.vm" );
		remote_vm = getTemplate( path, "remote.vm" );
		stub_vm = getTemplate( path, "stub.vm" );
		helper_vm = getTemplate( path, "helper.vm" );
		readme_vm = getTemplate( path, "readme.vm" );
		main_vm = getTemplate( path, "main.vm" );
		base_vm = getTemplate( path, "base.vm" );
		impl_vm = getTemplate( path, "impl.vm" );

		local_kwd = getPath( path, "javaKeywords.kwd" );
		etch_wireshark_vm = getTemplate(path, "etch_wireshark.vm");
	}

	private final Template vf_vm;

	private final Template intf_vm;

	private final Template remote_vm;

	private final Template stub_vm;

	private final Template helper_vm;

	private final Template readme_vm;

	private final Template main_vm;

	private final Template base_vm;

	private final Template impl_vm;

	private final String local_kwd;

	private LogHandler lh;
	
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
				lh.report( level == 2 ? LogHandler.LEVEL_WARNING : LogHandler.LEVEL_ERROR, null, msg );
			else
				System.out.printf( "Velocity msg (%d): %s\n", level, msg );
		}

		public void log( int level, String msg, Throwable e )
		{
			if (level < 2)
				return;
			
			if (lh != null)
				lh.report( level == 2 ? LogHandler.LEVEL_WARNING : LogHandler.LEVEL_ERROR, null, msg );
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
	public void generate( Module module, CmdLineOptions options )
		throws Exception
	{
		// java always wants to not flatten packages:
		options.noFlattenPackages = true;
		
		lh = options.lh;

		boolean ignoreGlobal = options.ignoreGlobalWordsList;
		boolean ignoreLocal = options.ignoreLocalWordsList;
		String userWords = options.userWordsList != null ? options.userWordsList.getPath() : null;
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
		ReservedWordChecker checker = new ReservedWordChecker( words, false, lh );
		module.treewalk( checker );
		if (!checker.ok())
		{
			lh.report( LogHandler.LEVEL_ERROR, null, "Encountered errors during java generation." );
			return;
		}

		// ok, we're ready to generate code. make sure the
		// output directories exist.

		Output dir = options.output;
		Output templateDir = options.templateOutput;
		
		String m = module.name().name;
		if (m.length() > 0)
		{
			dir = dir.newPackage( m );
			templateDir = templateDir.newPackage( m );
		}
		
		// generate code for each service.

		for (Service intf : module)
		{
			generate( intf, what, dir, templateDir );
		}
	}

	private void generate( final Service intf, Set<String> what, Output dir,
		Output templateDir ) throws Exception
	{
		what = populateWhat( what );

		if (what.isEmpty())
		{
			// lh.logMessage( lh.LEVEL_ERROR, null, "User has selected NONE\n" );
			return;
		}

		final MessageDirection msgDir = getMessageDirection( what );

		if (what.contains( WHAT_INTF ))
		{
			// Generate the value factory file.
			
			generateVf( intf, dir );
	
			// Generate the interface, remote, and stub files.
	
			generateIntfRemoteStub( intf, dir, msgDir, MessageDirection.BOTH, false );
	
			generateIntfRemoteStub( intf, dir, msgDir, MessageDirection.SERVER, true );
	
			generateIntfRemoteStub( intf, dir, msgDir, MessageDirection.CLIENT, true );
	
			// Generate helper file.
	
			generateHelper( intf, dir, msgDir );
			
			// Generate base file.
			
			generateBase( intf, dir, msgDir );
	
			// Generate readme file.
			
			generateReadme( intf, dir, msgDir );
			
			generateKeywordList(intf, dir, msgDir, MessageDirection.BOTH, true);
		}

		// Generate main template file.

		if (what.contains( WHAT_MAIN ))
			generateMain( intf, templateDir, msgDir );

		// Generate impl template file.

		if (what.contains( WHAT_IMPL ))
			generateImpl( intf, templateDir, msgDir );
	}

	private void generateReadme( final Service intf, Output dir,
		final MessageDirection msgDir ) throws Exception
	{
		doFile( dir, "readme-etch-java-files.txt", lh, new Gen()
		{
			public void run( PrintWriter pw ) throws Exception
			{
				generateReadme( pw, intf, msgDir );
			}
		} );
	}



	private final Template etch_wireshark_vm;
	
	private void generateKeywordList(final Service intf, Output dir,
			final MessageDirection what, final MessageDirection mc,
			final boolean hasBaseClass) throws Exception {
		doFile(dir, getKeywordFilename(intf), lh,
				new Gen() {
					public void run(PrintWriter pw) throws Exception {
						generateKeywords(pw, intf, mc, hasBaseClass);
					}
				});
	}
	
	void generateKeywords(PrintWriter pw, Service intf, MessageDirection mc,
			boolean hasBaseClass) throws Exception {
		VelocityContext context = new VelocityContext();
		context.put("now", new Date());
		context.put("version", VERSION);
		context.put("helper", this);
		context.put("intf", intf);
		context.put("mc", mc);
		context.put("suffix", MsgDirHelper.getSuffix(mc).toLowerCase());
		context.put("hasBaseClass", hasBaseClass);
		etch_wireshark_vm.merge(context, pw);
	}
	
	
	public String getKeywordForWireshark(String fieldname) {
		int hash = Hash.hash(fieldname);
		return String.format("0x%08x", hash) + "," + fieldname;
	}
	
	public String getKeywordFilename(Service intf) {
		return intf.name().name().toLowerCase() + "_wireshark.ewh";
	}
	
	
	private void generateVf( final Service intf, Output dir )
		throws Exception
	{
		doFile( dir, getVfName( intf ) + fnSuffix, lh, new Gen()
		{
			public void run( PrintWriter pw ) throws Exception
			{
				generateVf( pw, intf );
			}
		} );
	}

	private void generateHelper( final Service intf, Output dir,
		final MessageDirection msgDir ) throws Exception
	{
		doFile( dir, getHelperName( intf ) + fnSuffix, lh, new Gen()
		{
			public void run( PrintWriter pw ) throws Exception
			{
				generateHelper( pw, intf, msgDir );
			}
		} );
	}

	private void generateMain( final Service intf, Output dir,
		MessageDirection msgDir ) throws Exception
	{
		if (msgDir == MessageDirection.BOTH
				|| msgDir == MessageDirection.CLIENT)
			doFile( dir, getMainName( intf, MessageDirection.CLIENT ) + fnSuffix, lh, new Gen()
			{
				public void run( PrintWriter pw ) throws Exception
				{
					generateMain( pw, intf, MessageDirection.CLIENT, false );
				}
			} );

		if (msgDir == MessageDirection.BOTH
				|| msgDir == MessageDirection.SERVER)
			doFile( dir, getMainName( intf, MessageDirection.SERVER ) + fnSuffix, lh, new Gen()
			{
				public void run( PrintWriter pw ) throws Exception
				{
					generateMain( pw, intf, MessageDirection.SERVER, false );
				}
			} );
	}

	private void generateBase( final Service intf, Output dir,
		MessageDirection msgDir ) throws Exception
	{
		if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.CLIENT)
			doFile( dir, getBaseName( intf, MessageDirection.CLIENT ) + fnSuffix, lh, new Gen()
			{
				public void run( PrintWriter pw ) throws Exception
				{
					generateBase (pw, intf, MessageDirection.CLIENT, false );
				}
			} );

		if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.SERVER)
			doFile( dir, getBaseName( intf, MessageDirection.SERVER ) + fnSuffix, lh, new Gen()
			{
				public void run( PrintWriter pw ) throws Exception
				{
					generateBase (pw, intf, MessageDirection.SERVER, false );
				}
			} );
	}

	private void generateImpl( final Service intf, Output dir,
		MessageDirection msgDir ) throws Exception
	{
		if (msgDir == MessageDirection.BOTH
				|| msgDir == MessageDirection.CLIENT)
			doFile( dir, getImplName( intf, MessageDirection.CLIENT ) + fnSuffix, lh, new Gen()
			{
				public void run( PrintWriter pw ) throws Exception
				{
					generateImpl( pw, intf, MessageDirection.CLIENT, false );
				}
			} );

		if (msgDir == MessageDirection.BOTH
				|| msgDir == MessageDirection.SERVER)
			doFile( dir, getImplName( intf, MessageDirection.SERVER ) + fnSuffix, lh, new Gen()
			{
				public void run( PrintWriter pw ) throws Exception
				{
					generateImpl( pw, intf, MessageDirection.SERVER, false );
				}
			} );
	}

	private void generateIntfRemoteStub( final Service intf, Output dir,
		final MessageDirection what, final MessageDirection mc,
		final boolean hasBaseClass ) throws Exception
	{
		// Generate interface file

		doFile( dir, getIntfName( intf, mc ) + fnSuffix, lh, new Gen()
		{
			public void run( PrintWriter pw ) throws Exception
			{
				generateIntf( pw, intf, mc, hasBaseClass );
			}
		} );

		// Generate remote file

		if (mc == MessageDirection.BOTH || what == MessageDirection.BOTH
				|| mc != what)
			doFile( dir, getRemoteName( intf, mc ) + fnSuffix, lh, new Gen()
			{
				public void run( PrintWriter pw ) throws Exception
				{
					generateRemote( pw, intf, mc, hasBaseClass );
				}
			} );

		// Generate stub file

		if (mc == MessageDirection.BOTH || what == MessageDirection.BOTH
				|| mc == what)
			doFile( dir, getStubName( intf, mc ) + fnSuffix, lh, new Gen()
			{
				public void run( PrintWriter pw ) throws Exception
				{
					generateStub( pw, intf, mc, hasBaseClass );
				}
			} );
	}

	/**
	 * Generate the value factory for the service.
	 *
	 * @param pw
	 * @param intf
	 * @throws Exception
	 */
	void generateVf( PrintWriter pw, Service intf ) throws Exception
	{
		// params keeps track of the total set of parameters
		// named (for enums, structs, exceptions, and messages).
		Set<String> params = new HashSet<String>();

		VelocityContext context = new VelocityContext();
		context.put( "now", new Date() );
		context.put( "version", VERSION );
		context.put( "helper", this );
		context.put( "intf", intf );
		context.put( "params", params );
		vf_vm.merge( context, pw );
	}

	/**
	 * Generate the interface for the service.
	 *
	 * @param pw
	 * @param intf
	 * @param mc
	 * @param hasBaseClass
	 * @throws Exception
	 */
	void generateIntf( PrintWriter pw, Service intf, MessageDirection mc,
		boolean hasBaseClass ) throws Exception
	{
		VelocityContext context = new VelocityContext();
		context.put( "now", new Date() );
		context.put( "version", VERSION );
		context.put( "helper", this );
		context.put( "intf", intf );
		context.put( "mc", mc );
		context.put( "suffix", MsgDirHelper.getSuffix( mc ) );
		context.put( "hasBaseClass", hasBaseClass );
		intf_vm.merge( context, pw );
	}

	/**
	 * Generate the call to message implementation of the interface. This class
	 * turns calls on its methods into messages which are sent to the remote
	 * stub. For two-way calls, it then waits for a response message, returning
	 * the result therein to the caller.
	 *
	 * @param pw
	 * @param intf
	 * @param mc
	 * @param hasBaseClass
	 * @throws Exception
	 */
	void generateRemote( PrintWriter pw, Service intf, MessageDirection mc,
		boolean hasBaseClass ) throws Exception
	{
		VelocityContext context = new VelocityContext();
		context.put( "now", new Date() );
		context.put( "version", VERSION );
		context.put( "helper", this );
		context.put( "intf", intf );
		context.put( "mc", mc );
		context.put( "suffix", MsgDirHelper.getSuffix( mc ) );
		context.put( "hasBaseClass", hasBaseClass );
		context.put( "methodList", new ArrayList<String>());
		remote_vm.merge( context, pw );
	}

	/**
	 * Generate the message to call implementation. This class accepts a message
	 * and turns it back into a call on the user's implementation. For two-way
	 * messages, the return value from the user's implementation method is turned
	 * into the appropriate response message and sent.
	 *
	 * @param pw
	 * @param intf
	 * @param mc
	 * @param hasBaseClass
	 * @throws Exception
	 */
	void generateStub( PrintWriter pw, Service intf, MessageDirection mc,
		boolean hasBaseClass ) throws Exception
	{
		VelocityContext context = new VelocityContext();
		context.put( "now", new Date() );
		context.put( "version", VERSION );
		context.put( "helper", this );
		context.put( "intf", intf );
		context.put( "mc", mc );
		context.put( "suffix", MsgDirHelper.getSuffix( mc ) );
		context.put( "hasBaseClass", hasBaseClass );
		stub_vm.merge( context, pw );
	}

	/**
	 * Generate the transport plumbing helper.
	 *
	 * @param pw
	 * @param intf
	 * @param mc
	 * @throws Exception
	 */
	void generateHelper( PrintWriter pw, Service intf, MessageDirection mc )
		throws Exception
	{
		VelocityContext context = new VelocityContext();
		context.put( "now", new Date() );
		context.put( "version", VERSION );
		context.put( "helper", this );
		context.put( "intf", intf );
		context.put( "mc", mc );

		helper_vm.merge( context, pw );
	}

	/**
	 * Generate the readme.txt.
	 *
	 * @param pw
	 * @param intf
	 * @param mc
	 * @throws Exception
	 */
	void generateReadme( PrintWriter pw, Service intf, MessageDirection mc ) throws Exception
	{
		VelocityContext context = new VelocityContext();
		context.put( "now", new Date() );
		context.put( "version", VERSION );
		context.put( "helper", this );
		context.put( "intf", intf );
		context.put( "mc", mc );

		readme_vm.merge( context, pw );
	}

	/**
	 * Generate the template main program.
	 *
	 * @param pw
	 * @param intf
	 * @param mc
	 * @param hasBaseClass
	 * @throws Exception
	 */
	void generateMain( PrintWriter pw, Service intf, MessageDirection mc,
		boolean hasBaseClass ) throws Exception
	{
		VelocityContext context = new VelocityContext();
		context.put( "now", new Date() );
		context.put( "version", VERSION );
		context.put( "helper", this );
		context.put( "intf", intf );
		context.put( "mc", mc );
		context.put( "suffix", MsgDirHelper.getSuffix( mc ) );
		context.put( "hasBaseClass", hasBaseClass );
		main_vm.merge( context, pw );
	}

	/**
	 * Generates the base implementation of the interfaces, with each
	 * method throwing an exception to the tune that it isn't implemented.
	 * User's impl will extend this base implementation.
	 * @param pw
	 * @param intf
	 * @param mc
	 * @param hasBaseClass
	 * @throws Exception
	 */
	void generateBase( PrintWriter pw, Service intf, MessageDirection mc,
		boolean hasBaseClass ) throws Exception
		{
			VelocityContext context = new VelocityContext();
			context.put( "now", new Date() );
			context.put( "version", VERSION );
			context.put( "helper", this );
			context.put( "intf", intf );
			context.put( "mc", mc );
			context.put( "suffix", MsgDirHelper.getSuffix( mc ) );
			context.put( "hasBaseClass", hasBaseClass );
			context.put( "methodList", new ArrayList<String>());
			base_vm.merge( context, pw );
		}

	/**
	 * Generate the template user implemention class which extends the base
	 * implementation generated above. This class will only have the appropriate
	 * constructor and reference to the appropriate remote, and a comment inviting
	 * the user to override methods.
	 * @param pw
	 * @param intf
	 * @param mc
	 * @param hasBaseClass
	 * @throws Exception
	 */
	void generateImpl( PrintWriter pw, Service intf, MessageDirection mc,
		boolean hasBaseClass ) throws Exception
	{
		VelocityContext context = new VelocityContext();
		context.put( "now", new Date() );
		context.put( "version", VERSION );
		context.put( "helper", this );
		context.put( "intf", intf );
		context.put( "mc", mc );
		context.put( "suffix", MsgDirHelper.getSuffix( mc ) );
		context.put( "hasBaseClass", hasBaseClass );
		impl_vm.merge( context, pw );
	}

	private String getVfName( Service intf )
	{
		return "ValueFactory" + getIntfName( intf, MessageDirection.BOTH );
	}

	private String getIntfName( Service intf, MessageDirection mc )
	{
		String suffix = MsgDirHelper.getSuffix( mc );
		return intf.name() + suffix;
	}

	private String getMainName( Service intf, MessageDirection mc )
	{
		if (mc == MessageDirection.SERVER)
			return "Main" + intf.name() + "Listener";
		return "Main" + getIntfName( intf, mc );
	}

	private String getImplName( Service intf, MessageDirection mc )
	{
		return "Impl" + getIntfName( intf, mc );
	}

	private String getRemoteName( Service intf, MessageDirection mc )
	{
		return "Remote" + getIntfName( intf, mc );
	}

	private String getStubName( Service intf, MessageDirection mc )
	{
		return "Stub" + getIntfName( intf, mc );
	}

	private String getHelperName( Service intf )
	{
		return intf.name() + "Helper";
	}
	
	private String getBaseName( Service intf, MessageDirection mc )
	{
		return "Base" + getIntfName( intf, mc );
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
				return value.image + "l";
			case EtchGrammarConstants.FLOAT:
				return value.image + "f";
			case EtchGrammarConstants.DOUBLE:
				return value.image + "d";
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
	 * @return the fundamental native type for java. so etch int -> java int,
	 * while etch string -> java String.
	 */
	@Override
	public String getNativeTypeName( TypeRef type )
	{
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.VOID:
				return "void";
			case EtchGrammarConstants.BOOLEAN:
				return "boolean";
			case EtchGrammarConstants.BYTE:
				return "byte";
			case EtchGrammarConstants.SHORT:
				return "short";
			case EtchGrammarConstants.INT:
				return "int";
			case EtchGrammarConstants.LONG:
				return "long";
			case EtchGrammarConstants.FLOAT:
				return "float";
			case EtchGrammarConstants.DOUBLE:
				return "double";
			case EtchGrammarConstants.STRING:
				return "String";
			case EtchGrammarConstants.OBJECT:
				return "Object";
			default:
			{
				// we have to use a fully qualified name here.
				// find the actual type...
				Named<?> n = type.intf().get( t.image );
				if (n == null)
					throw new IllegalArgumentException( String.format(
						"undefined or ambiguous name at line %d: %s",
						t.beginLine, t.image ) );
				return n.efqname( this );
			}
		}
	}

	/**
	 * @param type the etch type
	 * @return the fundamental native reference type for java. so etch int ->
	 * java Integer, while etch string -> java String.
	 */
	private String getRefTypeName( TypeRef type )
	{
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.VOID:
				return "void";
			case EtchGrammarConstants.BOOLEAN:
				return "Boolean";
			case EtchGrammarConstants.BYTE:
				return "Byte";
			case EtchGrammarConstants.SHORT:
				return "Short";
			case EtchGrammarConstants.INT:
				return "Integer";
			case EtchGrammarConstants.LONG:
				return "Long";
			case EtchGrammarConstants.FLOAT:
				return "Float";
			case EtchGrammarConstants.DOUBLE:
				return "Double";
			case EtchGrammarConstants.STRING:
				return "String";
			case EtchGrammarConstants.OBJECT:
				return "Object";
			default:
			{
				// we have to use a fully qualified name here.
				// find the actual type...
				Named<?> n = type.intf().get( t.image );
				if (n == null)
					throw new IllegalArgumentException( String.format(
						"undefined or ambiguous name at line %d: %s",
						t.beginLine, t.image ) );
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

	@Override
	public String formatString( ParamList<Service> n, boolean isExcept )
		throws ParseException, IOException
	{
		ToString ts = (ToString) n.getOpt( "ToString" );
		List<FmtItem> list;
		if (ts != null)
		{
			list = ts.getFormat();
			n.checkFormatList( ts.lineno(), list );
		}
		else if (isExcept)
			list = n.mkFormatList( true, ((Except)n).hasExtends() );
		else
			list = n.mkFormatList( false, ((Struct)n).hasExtends() );

		if (list.size() == 1)
		{
			return list.get( 0 ).value();
		}

		StringBuffer sb = new StringBuffer();
		sb.append( "String.format( " );
		sb.append( "\"" );
		for (FmtItem i : list)
		{
			if (i instanceof FieldItem)
			{
				sb.append( "%s" );
			}
			else
			{
				escape( sb, ((StringItem) i).value() );
			}
		}
		sb.append( "\"" );
		for (FmtItem i : list)
		{
			if (i instanceof FieldItem)
			{
				sb.append( ", " );
				sb.append( ((FieldItem) i).value() );
			}
		}
		sb.append( " )" );
		return sb.toString();
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
	public String mfvname( String vname )
	{
		return "_mf_" + vname;
	}

	@Override
	public String mtvname( String vname )
	{
		return "_mt_" + vname;
	}

	@Override
	public String getLang()
	{
		return "java";
	}

	@Override
	public String enum_efqname( String fqname, String moduleName,
		String serviceName, String enumName )
	{
		return fqname;
	}

	@Override
	public String except_efqname( String fqname, String moduleName,
		String serviceName, String exceptName )
	{

		return fqname;
	}

	@Override
	public String struct_efqname( String fqname, String moduleName,
		String serviceName, String enumName )
	{

		return fqname;
	}

	@Override
	public String qualifyParameterName( Token name )
	{
		return name.image;
	}

	@Override
	public String qualifyConstantName( Service intf, Token name )
	{
		return intf.fqname() + '.' + name.image;
	}

	@Override
	public String qualifyEnumName( Service intf, Token name )
	{
		return intf.fqname() + '.' + name.image;
	}

	@Override
	public String getValidator( Named<?> named )
	{
		if (named instanceof Parameter)
		{
			Parameter param = (Parameter) named;
			TypeRef type = param.type();

			if (type.isBuiltin())
				return String.format( "Validator_%s.get( %d )",
					type.type(), type.dim() );

			Named<?> n = type.getNamed( type.intf() );

			if (n.isBuiltin())
			{
				Builtin b = (Builtin) n;
				String cn = b.className();
				
				int i = cn.indexOf( '<' );
				if (i >= 0)
					cn = cn.substring( 0, i );
				
				return String.format( "Validator_custom.get( %s.class, %d, %s )",
					cn, type.dim(), b.allowSubclass() );
			}

			// Allow subclassing for etch defined structs and externs.

			if (n.isStruct() || n.isExcept())
				return String.format( "Validator_custom.get( %s.class, %d, true )",
					n.efqname( this ), type.dim() );

			// Don't allow subclassing for externs or etch defined enums.

			if (!(n.isExtern() || n.isEnumx()))
				Assertion.check( n.isExtern() || n.isEnumx(),
					"n.isExtern() || n.isEnumx(): "+n );

			return String.format( "Validator_custom.get( %s.class, %d, false )",
				n.efqname( this ), type.dim() );
		}

		if (named instanceof Thrown)
		{
			Thrown thrown = (Thrown) named;
			Except e = (Except) thrown.getNamed();
			return String.format( "Validator_custom.get( %s.class, 0, true )", e.efqname( this ) );
		}

		if (named instanceof Item)
			return "Validator_boolean.get( 0 )";

		return "null";
	}

	/**
	 * @param name
	 * @return the appropriate name for a getter method.
	 */
	public String getGetterName( Name name )
	{
		String s = name.name;
		return "get"+s.substring( 0, 1 ).toUpperCase()+s.substring( 1 );
	}

	/**
	 * @param name
	 * @return the appropriate name for a setter method.
	 */
	public String getSetterName( Name name )
	{
		String s = name.name;
		return "set"+s.substring( 0, 1 ).toUpperCase()+s.substring( 1 );
	}

	@Override
	public void addDefaults( Service service ) throws ParseException
	{
		addBuiltin( service, newName( "List" ), "java.util.List<?>", true );
		addBuiltin( service, newName( "Map" ), "java.util.Map<?, ?>", true );
		addBuiltin( service, newName( "Set" ), "java.util.Set<?>", true );
		addBuiltin( service, newName( "Datetime" ), "java.util.Date", false );
	}
}
