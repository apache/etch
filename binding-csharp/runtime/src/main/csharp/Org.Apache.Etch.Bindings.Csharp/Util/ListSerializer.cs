// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using System.Collections;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;

namespace Org.Apache.Etch.Bindings.Csharp.Util
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

        public override StructValue ExportValue(ValueFactory vf, Object value)
        {
     //       List<object> list = (List<object>)value;
            ArrayList list = (ArrayList)value;
            Object[] values = new Object[ list.Count];
            int index = 0;

            foreach ( object me in list )
            {
                values[ index++ ] = me;
              
            }

            StructValue sv = new StructValue(type, vf);
            sv.Add( field, values );
            return sv;

        }

    }
}
