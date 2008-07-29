/* $Id$
 *
 * Created by sccomer on Mar 29, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.xml.compiler;

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
import etch.compiler.ast.Except;
import etch.compiler.ast.Item;
import etch.compiler.ast.Message;
import etch.compiler.ast.MessageDirection;
import etch.compiler.ast.Module;
import etch.compiler.ast.MsgDirHelper;
import etch.compiler.ast.Named;
import etch.compiler.ast.ParamList;
import etch.compiler.ast.Parameter;
import etch.compiler.ast.Service;
import etch.compiler.ast.Thrown;
import etch.compiler.ast.TypeRef;
import etch.compiler.opt.ToString;
import etch.compiler.opt.ToString.FieldItem;
import etch.compiler.opt.ToString.FmtItem;
import etch.compiler.opt.ToString.StringItem;

/**
 * Compiler is a helper class not only for Backend, but also
 * for the templates. They call methods here to perform
 * "hard" tasks.
 */
public class Compiler extends Backend
{
	private final static String tmplPath1 = "etch/bindings/xml/compiler/";
	private final static String tmplPath2 = "src/etch/bindings/xml/compiler/";

	private final static String fnSuffix = ".xml";

	private final static String VERSION = Version.VERSION + " / " + CompilerVersion.VERSION;

	/**
	 * Constructs the Compiler. This is a helper class not only
	 * for Backend, but also for the templates. They call methods
	 * here to perform "hard" tasks.
	 *
	 * @throws Exception
	 */
	public Compiler() throws Exception
	{
		initVelocity();
		String[] path = { tmplPath1, tmplPath2 };
		vf_vm = getTemplate( path, "vf.vm" );
	}

	private final Template vf_vm;

	@Override
	public void generate( Module module, Set<String> what,
		File dir, boolean ignoreGlobal, boolean ignoreLocal, String userWords )
		throws Exception
	{

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

		// System.out.println( "generating to "+dir );

		dir.mkdirs();

		// generate code for each service.
		MessageDirection msgDir = getMessageDirection( what );

		if (msgDir != null)
		{

			for (Service intf : module)
			{
				gIntf = intf;
				generate( intf, msgDir, dir );
			}
			gIntf = null;
		}
		else {
			System.err.printf(" User has selected an unknown option " );
			System.exit( 1 );
			return;
		}
	}

	private Service gIntf;

	private void generate( final Service intf, MessageDirection what, File dir )
		throws Exception
	{
		// Generate the value factory file

		doFile( dir, getVfName( intf )+fnSuffix, true,
			true, new Gen() { public void run( PrintWriter pw ) throws Exception
				{ generateVf( pw, intf ); } } );
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

	private String getVfName( Service intf )
	{
		return getIntfName( intf, MessageDirection.BOTH );
	}

	private String getIntfName( Service intf, MessageDirection mc )
	{
		String suffix = MsgDirHelper.getSuffix( mc );
		return intf.name()+suffix;
	}

	@Override
	public String asyncReceiverPoolName( Message msg )
	{
		return msg.getAsyncReceiver().toString().toLowerCase();
	}

	@Override
	public String getTypeValue( TypeRef type, Token value ) throws Exception
	{
//		System.out.println( "getTypeValue called with: "+type+": "+value );
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.STRING: return protectString( value.image );
			default: return value.image;
		}
	}

	private String protectString( String s ) throws Exception
	{
		return s;
		//return protectAttr(s);
	}

	/**
	 * @param type
	 * @return type name appropriate for use as a structure
	 * element or exception parameter or function parameter
	 * or result.
	 */
	@Override
	public String getTypeName( TypeRef type )
	{
		if (type.dim() > 0)
			return getNativeTypeName( type )+dim2spec( type.dim() );
		return getRefTypeName( type );
	}

	/**
	 * @param type the etch type
	 * @return the fundamental native type for java. so etch
	 * int -> java int, while etch string -> java String.
	 */
	@Override
	public String getNativeTypeName( TypeRef type )
	{
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.VOID: return "void";
			case EtchGrammarConstants.BOOLEAN: return "boolean";
			case EtchGrammarConstants.BYTE: return "byte";
			case EtchGrammarConstants.SHORT: return "short";
			case EtchGrammarConstants.INT: return "int";
			case EtchGrammarConstants.LONG: return "long";
			case EtchGrammarConstants.FLOAT: return "float";
			case EtchGrammarConstants.DOUBLE: return "double";
			case EtchGrammarConstants.STRING: return "String";
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
	 * @return the fundamental native reference type for java.
	 * so etch int -> java Integer, while etch string -> java String.
	 */
	private String getRefTypeName( TypeRef type )
	{
		Token t = type.type();
		switch (t.kind)
		{
			case EtchGrammarConstants.VOID: return "void";
			case EtchGrammarConstants.BOOLEAN: return "Boolean";
			case EtchGrammarConstants.BYTE: return "Byte";
			case EtchGrammarConstants.SHORT: return "Short";
			case EtchGrammarConstants.INT: return "Integer";
			case EtchGrammarConstants.LONG: return "Long";
			case EtchGrammarConstants.FLOAT: return "Float";
			case EtchGrammarConstants.DOUBLE: return "Double";
			case EtchGrammarConstants.STRING: return "String";
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
		else
			list = n.mkFormatList( isExcept );

		if (list.size() == 1)
		{
			return list.get( 0 ).value();
		}

		StringBuffer sb = new StringBuffer();
		sb.append( "String.format( " );
		sb.append( "\"" );
		for (FmtItem i: list)
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
		for (FmtItem i: list)
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
		return "_mf_"+vname;
	}

	@Override
	public String mtvname( String vname )
	{
		return "_mt_"+vname;
	}

	@Override
	public String getLang()
	{
		return "xml";
	}

	@Override
	public String enum_efqname( String fqname, String moduleName,
		String serviceName, String enumName )
	{
		return fqname;
	}

	@Override
	public String except_efqname(String fqname, String moduleName, String serviceName, String exceptName) {

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
		if (named instanceof Parameter)
		{
			Parameter param = (Parameter) named;
			TypeRef type = param.type();

			if (type.isBuiltin())
				return "Validator_"+type.type()+".get( "+type.dim()+" )";

			return "Validator_custom.getCustom( "+type.getNamed( gIntf ).efqname( this )+".class, "+type.dim()+" )";
		}

		if (named instanceof Thrown)
		{
			Thrown thrown = (Thrown) named;
			Except e = (Except) thrown.getNamed();
			return "Validator_custom.getCustom( "+e.efqname( this )+".class, 0 )";
		}

		if (named instanceof Item)
			return "Validator_boolean.get( 0 )";

		return "null";
	}

	/**
	 * @param descriptions
	 * @return string with descriptions
	 */
	public String emitXml(List<String> descriptions)
	{
		StringBuffer buffer = new StringBuffer();
		for(int i = 0; i < descriptions.size(); i++)
		{
			Object objMessage = descriptions.get(i);
			String message = "";
			if(objMessage != null)
			{
				message = objMessage.toString();
			}

			buffer.append(message);

			if(i < descriptions.size() - 1)
			{
				buffer.append("\r\n");
			}
		}

		return buffer.toString();
	}

	/**
	 * @param unprotected
	 * @return attribute value protected.
	 * @throws IOException
	 */
	public String protectAttr(String unprotected) throws IOException
	{
		return metreos.core.xml.XmlParser.ProtectAttrValue.toString(unprotected);
	}

	/**
	 * @param unprotected
	 * @return cdata value protected.
	 * @throws IOException
	 */
	public String protectCData(String unprotected) throws IOException
	{
		return metreos.core.xml.XmlParser.ProtectCData.toString(unprotected);
	}
}
