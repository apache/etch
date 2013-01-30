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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{ 
    /// <summary>
    /// Serialize a DateTime into our network standard format, which
    /// is the unix standard of milliseconds since jan 1, 1970 utc.
    /// </summary>
    public class DateSerializer : ImportExportHelper
    {
        private const String FIELD_NAME = "dateTime";

        private readonly XType type;
        private readonly Field field;

        public DateSerializer( XType type, Field field )
        {
            this.type = type;
            this.field = field;
        }
        
        /// <summary>
        /// Defines custom fields in the value factory so that the importer
        /// can find them
        /// </summary>
        /// <param name="type"></param>
        /// <param name="class2type"></param>
        public static void Init( XType type, Class2TypeMap class2type )
        {
            Field field = type.GetField( FIELD_NAME );
            class2type.Add( typeof( DateTime ), type );
            type.SetComponentType( typeof( DateTime ) );
            type.SetImportExportHelper( new DateSerializer( type, field ) );
            type.PutValidator( field, Validator_long.Get( 0 ) );
            type.Lock();
        }

        public override Object ImportValue( StructValue sv )
        {
            long ms = (long)sv.Get(field);
            return new DateTime(epoch + ms * TICKS_PER_MS, DateTimeKind.Utc).ToLocalTime();
        }

        public override StructValue ExportValue( ValueFactory vf, Object value )
        {
            long ms = (((DateTime)value).ToUniversalTime().Ticks - epoch) / TICKS_PER_MS;
            StructValue sv = new StructValue( type, vf );
            sv.Add(field, ms);
            return sv;
        }

        private const long TICKS_PER_MS = 10000;

        private static readonly long epoch =
            new DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind.Utc).Ticks;
    }
}
