package etch.compiler;

import java.io.IOException;

import etch.compiler.ast.Message;
import etch.compiler.ast.Module;
import etch.compiler.ast.Named;
import etch.compiler.ast.ParamList;
import etch.compiler.ast.Service;
import etch.compiler.ast.TypeRef;

/**
 * Compiler which doesn't generated any code.
 */
public class NullCompiler extends Backend
{
	/**
	 * Constructs the NullCompiler.
	 * @throws Exception
	 */
	public NullCompiler() throws Exception
	{
		// TODO Auto-generated constructor stub
	}

	@Override
	public void addDefaults( Service service ) throws ParseException
	{
		addBuiltin( service, newName( "List" ), "", true );
		addBuiltin( service, newName( "Map" ), "", true );
		addBuiltin( service, newName( "Set" ), "", true );
		addBuiltin( service, newName( "Datetime" ), "", false );
	}

	@Override
	public String asyncReceiverPoolName( Message msg )
	{
		// ignore
		return null;
	}

	@Override
	public String enum_efqname( String fqname, String moduleName,
		String serviceName, String enumName )
	{
		// ignore
		return null;
	}

	@Override
	public String except_efqname( String fqname, String moduleName,
		String serviceName, String enumName )
	{
		// ignore
		return null;
	}

	@Override
	public String formatString( ParamList<Service> n, boolean isExcept )
		throws ParseException, IOException
	{
		// ignore
		return null;
	}

	@Override
	public void generate( Module module, CmdLineOptions options )
		throws Exception
	{
		// ignore
	}

	@Override
	public String getLang()
	{
		// ignore
		return null;
	}

	@Override
	public String getNativeTypeName( TypeRef type )
	{
		// ignore
		return null;
	}

	@Override
	public String getTypeName( TypeRef type )
	{
		// ignore
		return null;
	}

	@Override
	public String getTypeValue( TypeRef type, Token value ) throws IOException,
		Exception
	{
		// ignore
		return null;
	}

	@Override
	public String getValidator( Named<?> named )
	{
		// ignore
		return null;
	}

	@Override
	public String mfvname( String vname )
	{
		// ignore
		return null;
	}

	@Override
	public String mtvname( String vname )
	{
		// ignore
		return null;
	}

	@Override
	public String qualifyConstantName( Service intf, Token name )
	{
		// ignore
		return null;
	}

	@Override
	public String qualifyEnumName( Service intf, Token name )
	{
		// ignore
		return null;
	}

	@Override
	public String qualifyParameterName( Token name )
	{
		// ignore
		return null;
	}

	@Override
	public String struct_efqname( String fqname, String moduleName,
		String serviceName, String enumName )
	{
		// ignore
		return null;
	}
}
