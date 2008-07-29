using System;
using System.Collections.Generic;
using System.Text;

using Etch.Support;
using Etch.Msg;


namespace Etch.Util
{
    /// <summary>
    /// An etch serializer for StrIntHashMap.
    /// </summary>
    public class StrIntHashMapSerializer
    {
        private static String FIELD_NAME = "keysAndValues";

        public static StrIntHashMap ImportValue( ValueFactory vf, StructValue sv )
        {
            StrIntHashMap map = new StrIntHashMap();
            Object[] av = ( Object[] ) sv.Get( vf.GetField( FIELD_NAME ) );
            int n = av.Length;
            int index = 0;
            while ( index < n )
            {
                string key = ( String ) av[ index++ ];
                int? value = ( int? ) av[ index++ ];
                map.Add( key, value );
            }
            
            return map;
        }

        public static void ExportValue( ValueFactory vf, StrIntHashMap value,
            StructValue sv )
        {
            Object[] vals = new Object[ value.Count*2 ];
            int index = 0;

            foreach( KeyValuePair<String, int?> me in value)
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
