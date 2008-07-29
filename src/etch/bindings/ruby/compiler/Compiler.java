/* $Id$
 *
 * Created by sccomer on Mar 29, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.ruby.compiler;

import java.io.File;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringReader;
import java.util.Date;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import org.apache.velocity.Template;
import org.apache.velocity.VelocityContext;

import etch.compiler.Backend;
import etch.compiler.EtchGrammarConstants;
import etch.compiler.ParseException;
import etch.compiler.Token;
import etch.compiler.Version;
import etch.compiler.ast.Message;
import etch.compiler.ast.MessageDirection;
import etch.compiler.ast.Module;
import etch.compiler.ast.MsgDirHelper;
import etch.compiler.ast.Named;
import etch.compiler.ast.ParamList;
import etch.compiler.ast.Service;
import etch.compiler.ast.TypeRef;
import etch.compiler.opt.ToString;
import etch.compiler.opt.ToString.FieldItem;
import etch.compiler.opt.ToString.FmtItem;
import etch.compiler.opt.ToString.StringItem;

/**
 * Description of Compiler.
 */
public class Compiler extends Backend
{
	private final static String tmplPath1 = "etch/bindings/ruby/compiler/";
	private final static String tmplPath2 = "src/etch/bindings/ruby/compiler/";

	private final static String fnSuffix = ".rb";

	private final static String VERSION = Version.VERSION + " / " + CompilerVersion.VERSION;

	/**
	 * Constructs the Compiler.
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
	}

	private final Template vf_vm;

	private final Template intf_vm;

	private final Template remote_vm;

	private final Template stub_vm;

	@Override
	public void generate( Module module, Set<String> type, File dir,
		boolean ignoreGlobal, boolean ignoreLocal, String userWords )
		throws Exception
	{
		if (module.name().name.length() > 0)
		{
			String path = module.name().name.replace( '.', '/' );
			dir = new File( dir, path );
		}

		dir.mkdirs();

		for (Service intf: module)
			generate( intf, dir );
	}

	private void generate( final Service intf, File dir ) throws Exception
	{
		// Generate the value factory file

		doFile( dir, getVfName( intf )+fnSuffix, true,
			true, new Gen() { public void run( PrintWriter pw ) throws Exception
				{ generateVf( pw, intf ); } } );

		// Generate the interface, remote, and stub files

		boolean hasBaseClass = intf.hasMessageDirection( MessageDirection.BOTH );

		generate( intf, dir, MessageDirection.BOTH, false, hasBaseClass );
		generate( intf, dir, MessageDirection.SERVER, hasBaseClass,
			intf.hasMessageDirection( MessageDirection.SERVER ) );
		generate( intf, dir, MessageDirection.CLIENT, hasBaseClass,
			intf.hasMessageDirection( MessageDirection.CLIENT ) );
	}

	private void generate( final Service intf, File dir, final MessageDirection mc,
		final boolean hasBaseClass, boolean makeFile ) throws Exception
	{
		// Generate interface file

		doFile( dir, getIntfName( intf, mc )+fnSuffix, makeFile,
			true, new Gen() { public void run( PrintWriter pw ) throws Exception
				{ generateIntf( pw, intf, mc, hasBaseClass ); } } );

		// Generate remote file

		doFile( dir, getRemoteName( intf, mc )+fnSuffix, makeFile,
			true, new Gen() { public void run( PrintWriter pw ) throws Exception
				{ generateRemote( pw, intf, mc, hasBaseClass ); } } );

		// Generate stub file

		doFile( dir, getStubName( intf, mc )+fnSuffix, makeFile,
			true, new Gen() { public void run( PrintWriter pw ) throws Exception
				{ generateStub( pw, intf, mc, hasBaseClass ); } } );
	}

	/**
	 * Generate the value factory for the service.
	 * @param pw
	 * @param intf
	 * @throws Exception
	 */
	void generateVf( PrintWriter pw, Service intf )
		throws Exception
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
	 * Generate the call to message implementation of the
	 * interface.
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
		remote_vm.merge( context, pw );
	}

	/**
	 * Generate the message to call implementation.
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

	private String getVfName( Service intf )
	{
		return "ValueFactory"+getIntfName( intf, MessageDirection.BOTH );
	}

	private String getIntfName( Service intf, MessageDirection mc )
	{
		String suffix = MsgDirHelper.getSuffix( mc );
		return intf.name()+suffix;
	}

	private String getRemoteName( Service intf, MessageDirection mc )
	{
		return "Remote"+getIntfName( intf, mc );
	}

	private String getStubName( Service intf, MessageDirection mc )
	{
		return "Stub"+getIntfName( intf, mc );
	}

	@Override
	public String asyncReceiverPoolName( Message msg )
	{
		return msg.getAsyncReceiver().toString().toLowerCase();
	}

	@Override
	public String getTypeValue( TypeRef type, Token value )
	{
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.LONG: return value.image+"l";
			case EtchGrammarConstants.FLOAT: return value.image+"f";
			case EtchGrammarConstants.DOUBLE: return value.image+"d";
			default: return value.image;
		}
	}

	@Override
	public String getTypeName( TypeRef type )
	{
		if (type.dim() > 0)
			return getNativeTypeName( type )+dim2spec( type.dim() );
		return getRefTypeName( type );
	}

	@Override
	public String getNativeTypeName( TypeRef type )
	{
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.VOID: return "Void";
			case EtchGrammarConstants.BOOLEAN: return "Boolean";
			case EtchGrammarConstants.BYTE: return "Byte";
			case EtchGrammarConstants.SHORT: return "Short";
			case EtchGrammarConstants.INT: return "Integer";
			case EtchGrammarConstants.LONG: return "Long";
			case EtchGrammarConstants.FLOAT: return "Float";
			case EtchGrammarConstants.DOUBLE: return "Double";
			case EtchGrammarConstants.STRING: return "String";
			default: return t.image;
		}
	}

	private String getRefTypeName( TypeRef type )
	{
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.VOID: return "Void";
			case EtchGrammarConstants.BOOLEAN: return "Boolean";
			case EtchGrammarConstants.BYTE: return "Byte";
			case EtchGrammarConstants.SHORT: return "Short";
			case EtchGrammarConstants.INT: return "Integer";
			case EtchGrammarConstants.LONG: return "Long";
			case EtchGrammarConstants.FLOAT: return "Float";
			case EtchGrammarConstants.DOUBLE: return "Double";
			case EtchGrammarConstants.STRING: return "String";
			default: return t.image;
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
		else
			list = n.mkFormatList( isExcept );

		if (list.size() == 1)
		{
			return list.get( 0 ).value();
		}

		StringBuffer sb = new StringBuffer();
		sb.append( "\"" );
		for (FmtItem i: list)
		{
			if (i instanceof FieldItem)
			{
				sb.append( String.format( "#{%s}", i.value() ) );
			}
			else
			{
				escape( sb, ((StringItem) i).value() );
			}
		}
		sb.append( "\"" );
		return sb.toString();
	}

	private void escape( StringBuffer sb, String s ) throws IOException
	{
		StringReader rdr = new StringReader( s );
		int c;
		while ((c = rdr.read()) >= 0)
		{
			if (c == '"') sb.append( "\\\"" );
			else if (c == '\\') sb.append( "\\\\" );
			else if (c == '\t') sb.append( "\\t" );
			else if (c == '\r') sb.append( "\\r" );
			else if (c == '\n') sb.append( "\\n" );
			else sb.append( (char) c );
		}
	}

	@Override
	public String mfvname( String vname )
	{
		return "MF_"+vname;
	}

	@Override
	public String mtvname( String vname )
	{
		return "MT_"+vname;
	}

	@Override
	public String getLang()
	{
		return "ruby";
	}

	@Override
	public String enum_efqname( String fqname, String moduleName, String serviceName, String enumName )
	{
		return fqname;
	}

	@Override
	public String except_efqname(String fqname, String moduleName, String serviceName, String enumName) {

		return fqname;
	}

	@Override
	public String struct_efqname(String fqname, String moduleName, String serviceName, String enumName) {

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

		return intf.fqname()+'.'+name.image;
	}

	@Override
	public String qualifyEnumName( Service intf, Token name )
	{

		return intf.fqname()+'.'+name.image;
	}

	@Override
    public String getValidator( Named<?> named )
    {
	    // TODO Auto-generated method stub
	    return null;
    }
}
