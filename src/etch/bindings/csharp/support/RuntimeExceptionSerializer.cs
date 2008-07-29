using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;

namespace Etch.Support
{
    public class RuntimeExceptionSerializer : ImportExportHelper
    {
         private readonly static  String FIELD_NAME = "msg";
	
	public static void Init( XType type, Class2TypeMap class2type )
	{
		Field field = type.GetField( FIELD_NAME );
		class2type.Add(typeof(_Etch_RuntimeException), type );
		type.SetComponentType( typeof(_Etch_RuntimeException));
		type.SetImportExportHelper( new RuntimeExceptionSerializer( type, field ) );
		type.PutValidator( field, Validator_string.Get( 0 ) );
		type.Lock();
	}
	
	public RuntimeExceptionSerializer( XType type, Field field )
	{
		this.type = type;
		this.field = field;
	}
	
	private readonly XType type;
	
	private readonly Field field;
	
	public override StructValue ExportValue( Object value )
	{
		StructValue sv = new StructValue( type );
		sv.Add( field, value.ToString() );
		return sv;
	}

	public override Object ImportValue( StructValue sv )
	{
		sv.CheckType( type );
        return new _Etch_RuntimeException((String)sv.Get(field));
	}
    }
}
