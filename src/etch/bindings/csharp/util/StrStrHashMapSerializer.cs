using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using Etch.Support;

namespace Etch.Util
{
    /// <summary>
    /// An etch serializer for StrStrHashMap.
    /// </summary>
    public class StrStrHashMapSerializer
    {
        private static String FIELD_NAME = "keysAndValues";

        public static StrStrHashMap ImportValue( ValueFactory vf, StructValue sv )
        {
            StrStrHashMap map = new StrStrHashMap();
            Object[] av = ( Object[] ) sv.Get( vf.GetField( FIELD_NAME ) );
            int n = av.Length;
            int index = 0;
            while ( index < n )
            {
                string key = ( String ) av[ index++ ];
                string value = ( String ) av[ index++ ];
                map.Add( key, value );
            }

            return map;
        }

        public static void ExportValue( ValueFactory vf, StrStrHashMap value,
            StructValue sv )
        {
            Object[] vals = new Object[ value.Count*2 ];
            int index = 0;

            foreach ( KeyValuePair<String, String> me in value )
            {
                vals[ index++ ] = me.Key;
                vals[ index++ ] = me.Value;
            }
            sv.Add( vf.GetField( FIELD_NAME ), vals );

        }

        /// <summary>
        /// Defines custom fields in the value factory so that the importer
        /// can find them.
        /// </summary>
        /// <param name="vf"></param>
        /// <param name="type"></param>
        public static void Init( FieldMap fields, XType type )
        {
            Field field = fields.Get( FIELD_NAME );
            type.PutValidator( field, Validator_object.Get( 1 ) );
        }
    }
}
