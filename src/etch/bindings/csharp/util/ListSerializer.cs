using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using Etch.Support;

namespace Etch.Util
{
    public class ListSerializer : ImportExportHelper
    {
        private static String FIELD_NAME = "values";

         /// <summary>
        /// Defines custom fields in the value factory so that the importer
        /// can find them.
        /// </summary>
        /// <param name="type"></param>
        /// <param name="class2type"></param>
        public static void Init( XType type, Class2TypeMap class2type )
        {
            Field field = type.GetField( FIELD_NAME );
     /*       class2type.Add( typeof( List<object> ), type );
            type.SetClass(typeof(List<object>)); */
            class2type.Add(typeof(ArrayList), type);
            type.SetComponentType(typeof(ArrayList));
            type.SetImportExportHelper( new ListSerializer( type, field ) );
            type.PutValidator( field, Validator_object.Get( 1 ) );
            type.Lock();
        }

        public ListSerializer( XType type, Field field )
        {
            this.type = type;
            this.field = field;
        }

        private readonly XType type;
        private readonly Field field;


        public override Object ImportValue( StructValue sv )
        {
       //    List<object> list = new List<object>();
            ArrayList list = new ArrayList();
            Object[] values = ( Object[] ) sv.Get( field );
            int n = values.Length;
            int index = 0;
            while ( index < n )
            {
               
                object value = ( object ) values[ index++ ];
                list.Add( value );
            }

            return list;
        }

        public override StructValue ExportValue( Object value )
        {
     //       List<object> list = (List<object>)value;
            ArrayList list = (ArrayList)value;
            Object[] values = new Object[ list.Count];
            int index = 0;

            foreach ( object me in list )
            {
                values[ index++ ] = me;
              
            }

            StructValue sv = new StructValue( type );
            sv.Add( field, values );
            return sv;

        }

    }
}
