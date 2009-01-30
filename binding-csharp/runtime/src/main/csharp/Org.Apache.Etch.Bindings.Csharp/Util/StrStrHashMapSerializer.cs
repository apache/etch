// $Id$
// 
// Copyright 2007-2008 Cisco Systems Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using System;
using System.Collections.Generic;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;

namespace Org.Apache.Etch.Bindings.Csharp.Util
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

        public override StructValue ExportValue(ValueFactory vf, Object value)
        {
            StrStrHashMap map = ( StrStrHashMap ) value;

            Object[] keysAndValues = new Object[ map.Count * 2 ];
            int index = 0;

            foreach ( KeyValuePair<String, String> me in map )
            {
                keysAndValues[ index++ ] = me.Key;
                keysAndValues[ index++ ] = me.Value;
            }

            StructValue sv = new StructValue(type, vf);
            sv.Add( field, keysAndValues );
            return sv;

        }

    }
}
