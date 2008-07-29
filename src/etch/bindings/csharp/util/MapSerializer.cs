using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using Etch.Support;

namespace Etch.Util
{
    public class MapSerializer : ImportExportHelper
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
      /*      class2type.Add( typeof( Dictionary<object,object> ), type );
            type.SetClass(typeof(Dictionary<object, object>)); */
            class2type.Add(typeof(Hashtable), type);
            type.SetComponentType(typeof(Hashtable));
            type.SetImportExportHelper( new MapSerializer( type, field ) );
            type.PutValidator( field, Validator_object.Get( 1 ) );
            type.Lock();
        }

        public MapSerializer( XType type, Field field )
        {
            this.type = type;
            this.field = field;
        }

        private readonly XType type;
        private readonly Field field;


        public override Object ImportValue( StructValue sv )
        {
     //      Dictionary<object,object> map = new Dictionary<object,object>();
           Hashtable map = new Hashtable();
            Object[] keysAndValues = ( Object[] ) sv.Get( field );
            int n = keysAndValues.Length;
            int index = 0;
            while ( index < n )
            {
                object key = ( object ) keysAndValues[ index++ ];
                object value = ( object ) keysAndValues[ index++ ];
                map.Add( key, value );
            }

            return map;
        }

        public override StructValue ExportValue( Object value )
        {
       //     Dictionary<object, object> map = (Dictionary<object, object>)value;

            Hashtable map = (Hashtable)value;

            Object[] keysAndValues = new Object[ map.Count * 2];
            int index = 0;

            foreach ( DictionaryEntry me in map )
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
