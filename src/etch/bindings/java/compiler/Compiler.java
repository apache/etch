/*
 * $Id$
 *
 * Created by sccomer on Mar 29, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.compiler;

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

import etch.compiler.Backend;
import etch.compiler.CmdLineOptions;
import etch.compiler.EtchGrammarConstants;
import etch.compiler.LogHandler;
import etch.compiler.ParseException;
import etch.compiler.Token;
import etch.compiler.Version;
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
	private final static String tmplPath1 = "etch/bindings/java/compiler/";

	private final static String tmplPath2 = "src/etch/bindings/java/compiler/";

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
		String[] path =
		{
			tmplPath1, tmplPath2
		};
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
			String path = module.name().name.replace( '.', '/' );
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
			gIntf = intf;
			generate( intf, what, dir );
		}
		gIntf = null;
	}

	private Service gIntf;

	private void generate( final Service intf, Set<String> what, File dir )
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

		// Generate the value factory file
		doFile( dir, getVfName( intf ) + fnSuffix, true, true, new Gen()
		{
			public void run( PrintWriter pw ) throws Exception
			{
				generateVf( pw, intf );
			}
		}, lh );

		// Generate the interface, remote, and stub files

//		boolean hasBaseClass = intf.hasMessageDirection( MessageDirection.BOTH );

		generate( intf, dir, msgDir, MessageDirection.BOTH, false, true );

		generate( intf, dir, msgDir, MessageDirection.SERVER, true, true );

		generate( intf, dir, msgDir, MessageDirection.CLIENT, true, true );

		boolean force = what.contains( WHAT_FORCE );
//		System.out.println( "force = "+force );

		// Always generate helper file.

		doFile( dir, getHelperName( intf ) + fnSuffix, true, true, new Gen()
		{
			public void run( PrintWriter pw ) throws Exception
			{
				generateHelper( pw, intf, msgDir );
			}
		}, lh );

		// Always generate readme file.

		doFile( dir, "readme-etch-java-files.txt", true, true, new Gen()
		{
			public void run( PrintWriter pw ) throws Exception
			{
				generateReadme( pw, intf, msgDir );
			}
		}, lh );
		
		// Always Generate the base file
		generateBase( intf, dir, msgDir, true );

		// Generate main file.

		if (what.contains( WHAT_MAIN ))
			generateMain( intf, dir, msgDir, force );

		// Generate base and impl files.

		if (what.contains( WHAT_IMPL ))			
			generateImpl( intf, dir, msgDir, force );
	}

	private void generateMain( final Service intf, File dir,
		MessageDirection msgDir, boolean force ) throws Exception
	{
		if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.CLIENT)
			doFile( dir, getMainName( intf, MessageDirection.CLIENT ) + fnSuffix, true, force,
				new Gen()
				{
					public void run( PrintWriter pw ) throws Exception
					{
						generateMain (pw, intf, MessageDirection.CLIENT, false );
					}
				}, lh
			);

		if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.SERVER)
			doFile( dir, getMainName( intf, MessageDirection.SERVER ) + fnSuffix, true, force,
				new Gen()
				{
					public void run( PrintWriter pw ) throws Exception
					{
						generateMain (pw, intf, MessageDirection.SERVER, false );
					}
				}, lh
			);
	}

	private void generateBase( final Service intf, File dir,
		MessageDirection msgDir, boolean force ) throws Exception
	{
		if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.CLIENT)
			doFile( dir, getBaseName( intf, MessageDirection.CLIENT ) + fnSuffix, true, force,
				new Gen()
				{
					public void run( PrintWriter pw ) throws Exception
					{
						generateBase (pw, intf, MessageDirection.CLIENT, false );
					}
				}, lh
			);

		if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.SERVER)
			doFile( dir, getBaseName( intf, MessageDirection.SERVER ) + fnSuffix, true, force,
				new Gen()
				{
					public void run( PrintWriter pw ) throws Exception
					{
						generateBase (pw, intf, MessageDirection.SERVER, false );
					}
				}, lh
			);
	}

	private void generateImpl( final Service intf, File dir,
		MessageDirection msgDir, boolean force ) throws Exception
	{
		if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.CLIENT)
			doFile( dir, getImplName( intf, MessageDirection.CLIENT ) + fnSuffix, true, force,
				new Gen()
				{
					public void run( PrintWriter pw ) throws Exception
					{
						generateImpl (pw, intf, MessageDirection.CLIENT, false );
					}
				}, lh
			);

		if (msgDir == MessageDirection.BOTH || msgDir == MessageDirection.SERVER)
			doFile( dir, getImplName( intf, MessageDirection.SERVER ) + fnSuffix, true, force,
				new Gen()
				{
					public void run( PrintWriter pw ) throws Exception
					{
						generateImpl (pw, intf, MessageDirection.SERVER, false );
					}
				}, lh
			);
	}

	private void generate( final Service intf, File dir,
		final MessageDirection what, final MessageDirection mc,
		final boolean hasBaseClass, boolean makeFile ) throws Exception
	{

		// Generate interface file

		doFile( dir, getIntfName( intf, mc ) + fnSuffix, makeFile, true, new Gen()
		{
			public void run( PrintWriter pw ) throws Exception
			{
				generateIntf( pw, intf, mc, hasBaseClass );
			}
		}, lh );

		// Generate remote file

		if (mc == MessageDirection.BOTH || what == MessageDirection.BOTH
			|| mc != what)
			doFile( dir, getRemoteName( intf, mc ) + fnSuffix, makeFile,
				true, new Gen()
				{
					public void run( PrintWriter pw ) throws Exception
					{
						generateRemote( pw, intf, mc, hasBaseClass );
					}
				}, lh
			);

		// Generate stub file

		if (mc == MessageDirection.BOTH || what == MessageDirection.BOTH
			|| mc == what)
			doFile( dir, getStubName( intf, mc ) + fnSuffix, makeFile,
				true, new Gen()
				{
					public void run( PrintWriter pw ) throws Exception
					{
						generateStub( pw, intf, mc, hasBaseClass );
					}
				}, lh
			);
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
	 * Generate the call to message implementation of the interface.
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
		context.put("methodList", new ArrayList());
		remote_vm.merge( context, pw );
	}

	/**
	 * Generate the message to call implementation.
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
	 * Generate the transport helper.
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
	 * Generate the message to call implementation.
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
	 * Generates the base class to be used by the impl class
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
			context.put("methodList", new ArrayList());
			
			base_vm.merge( context, pw );
		}

	/**
	 * Generate the sample implementation.
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

	private String getMainName( Service intf, MessageDirection mc )
	{
		if (mc == MessageDirection.SERVER)
			return "Main" + intf.name() + "Listener";
		return "Main" + getIntfName(intf, mc);
	}
	private String getBaseName( Service intf, MessageDirection mc )
	{
		return "Base" + getIntfName( intf, mc );
	}

	private String getImplName( Service intf, MessageDirection mc )
	{
		return "Impl" + getIntfName( intf, mc );
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
				Named<?> n = gIntf.get( t.image );
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
				Named<?> n = gIntf.get( t.image );
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

			Named<?> n = type.getNamed( gIntf );

			// Allow subclassing for builtins if they want it

			if (n.isBuiltin())
			{
				Builtin b = (Builtin) n;
				return String.format( "Validator_custom.get( %s.class, %d, %s )",
					b.className(), type.dim(), b.allowSubclass() );
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
		addBuiltin( service, newName( "List" ), "java.util.List", true );
		addBuiltin( service, newName( "Map" ), "java.util.Map", true );
		addBuiltin( service, newName( "Set" ), "java.util.Set", true );
		addBuiltin( service, newName( "Datetime" ), "java.util.Date", false );
 	}
}
