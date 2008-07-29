using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;
using Etch.Msg;

namespace Etch.Util
{
    public class DateSerializer
    {
        private readonly static String FIELD_NAME = "dateTime";

        public static void Init( FieldMap fields, XType mt )
        {
            Field field = fields.Get( FIELD_NAME );
            mt.PutValidator( field, Validator_long.Get( 0 ) );
        }

        public static void ExportValue( ValueFactory vf, DateTime date, StructValue sv )
        {
            sv.Add( vf.GetField( FIELD_NAME ), date.Ticks );
        }

        public static Object ImportValue( ValueFactory vf, StructValue sv )
        {
            // TODO
            return null;
        }
    }
}
