using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;

namespace Etch.Support
{
    public class AuthExceptionSerializer : ImportExportHelper
    {
        private readonly static  String FIELD_NAME = "msg";
	
	    public static void Init( XType type, Class2TypeMap class2type )
	    {
		    Field field = type.GetField( FIELD_NAME );
		    class2type.Add(typeof(_Etch_AuthException), type );
		    type.SetComponentType( typeof(_Etch_AuthException));
		    type.SetImportExportHelper( new AuthExceptionSerializer( type, field ) );
		    type.PutValidator( field, Validator_string.Get( 0 ) );
		    type.Lock();
	    }
	
	    public AuthExceptionSerializer( XType type, Field field )
	    {
		    this.type = type;
		    this.field = field;
	    }
	
	    private readonly XType type;
	
	    private readonly Field field;
	
	    public StructValue exportValue( Object value )
	    {
		    StructValue sv = new StructValue( type );
		    sv.Add( field, value.ToString() );
		    return sv;
	    }

	    public Object importValue( StructValue sv )
	    {
		    sv.CheckType( type );
		    return new _Etch_AuthException( (String) sv.Get( field ) );
	    }

    }
}
