using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using Etch.Support;

namespace Etch.Util
{
    public class URLSerializer : ImportExportHelper
    {
        private readonly static String FIELD_NAME = "urlStr";

        public static void Init( XType type, Class2TypeMap class2type )
        {
            Field field = type.GetField( FIELD_NAME );
            class2type.Add( typeof( URL ), type );
            type.SetComponentType( typeof( URL ) );
            type.SetImportExportHelper( new URLSerializer( type, field ) );
            type.PutValidator( field, Validator_string.Get( 0 ) );
            type.Lock();
        }

        public URLSerializer( XType type, Field field )
        {
            this.type = type;
            this.field = field;
        }

        private readonly XType type;
        private readonly Field field;

        public override Object ImportValue( StructValue sv )
	    {
		    return new URL( (String) sv.Get( field ) );
	    }

	    public override StructValue ExportValue( Object value )
	    {
		    StructValue sv = new StructValue( type );
		    sv.Add( field, ((URL) value).ToString() );
		    return sv;
	    }
    }
}
