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
    public class StrIntHashMapSerializer : ImportExportHelper
    {
        private static String FIELD_NAME = "keysAndValues";

        /// <summary>
        /// Defines custom fields in the value factory so that the importer
        /// can find them.
        /// </summary>
        /// <param name="type"></param>
        ///  <param name="class2type"></param>
        public static void Init( XType type, Class2TypeMap class2type )
        {
            Field field = type.GetField( FIELD_NAME );
            class2type.Add( typeof( StrIntHashMap ), type );
            type.SetComponentType( typeof( StrIntHashMap ) );
            type.SetImportExportHelper( new StrIntHashMapSerializer( type, field ) );
            type.PutValidator( field, Validator_object.Get( 1 ) );
            type.Lock();
        }

        public StrIntHashMapSerializer( XType type, Field field )
        {
            this.type = type;
            this.field = field;
        }

        private readonly XType type;
        private readonly Field field;

        public override Object ImportValue( StructValue sv )
        {
            StrIntHashMap map = new StrIntHashMap();

            Object[] keysAndValues = ( Object[] ) sv.Get( field );
            int n = keysAndValues.Length;
            int index = 0;
            while ( index < n )
            {
                string key = ( String ) keysAndValues[ index++ ];
                int? value = ( int? ) keysAndValues[ index++ ];
                map.Add( key, value );
            }
            
            return map;
        }

        public override StructValue ExportValue( Object value )
        {
            StrIntHashMap map = ( StrIntHashMap ) value;

            Object[] keysAndValues = new Object[ map.Count * 2 ];
            int index = 0;

            foreach( KeyValuePair<String, int?> me in map )
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
