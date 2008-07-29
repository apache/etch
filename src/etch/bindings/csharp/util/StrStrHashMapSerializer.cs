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
    public class StrStrHashMapSerializer : ImportExportHelper
    {
        private static String FIELD_NAME = "keysAndValues";

        /// <summary>
        /// Defines custom fields in the value factory so that the importer
        /// can find them.
        /// </summary>
        /// <param name="type"></param>
        /// <param name="class2type"></param>
        public static void Init( XType type, Class2TypeMap class2type )
        {
            Field field = type.GetField( FIELD_NAME );
            class2type.Add( typeof( StrStrHashMap ), type );
            type.SetComponentType( typeof( StrStrHashMap ) );
            type.SetImportExportHelper( new StrStrHashMapSerializer( type, field ) );
            type.PutValidator( field, Validator_object.Get( 1 ) );
            type.Lock();
        }

        public StrStrHashMapSerializer( XType type, Field field )
        {
            this.type = type;
            this.field = field;
        }

        private readonly XType type;
        private readonly Field field;


        public override Object ImportValue( StructValue sv )
        {
            StrStrHashMap map = new StrStrHashMap();

            Object[] keysAndValues = ( Object[] ) sv.Get( field );
            int n = keysAndValues.Length;
            int index = 0;
            while ( index < n )
            {
                String key = ( String ) keysAndValues[ index++ ];
                String value = ( String ) keysAndValues[ index++ ];
                map.Add( key, value );
            }

            return map;
        }

        public override StructValue ExportValue( Object value )
        {
            StrStrHashMap map = ( StrStrHashMap ) value;

            Object[] keysAndValues = new Object[ map.Count * 2 ];
            int index = 0;

            foreach ( KeyValuePair<String, String> me in map )
            {
                keysAndValues[ index++ ] = me.Key;
                keysAndValues[ index++ ] = me.Value;
            }

            StructValue sv = new StructValue( type );
            sv.Add( field, keysAndValues );
            return sv;

        }

    }
}
