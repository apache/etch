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
using Etch.Msg;
using Etch.Support;

namespace Etch.Util
{
    public class URLSerializer : ImportExportHelper
    {
        private const String FIELD_NAME = "urlStr";

        public static void Init( XType type, Class2TypeMap class2type )
        {
            Field field = type.GetField( FIELD_NAME );
            class2type.Add( typeof( URL ), type );
            type.SetComponentType( typeof( URL ) );
            type.SetImportExportHelper( new URLSerializer( type, field ) );
            type.PutValidator( field, Validator_string.Get( 0 ) );
            type.Lock();
        }

        public URLSerializer( XType type, Field field )
        {
            this.type = type;
            this.field = field;
        }

        private readonly XType type;
        private readonly Field field;

        public override Object ImportValue( StructValue sv )
	    {
		    return new URL( (String) sv.Get( field ) );
	    }

        public override StructValue ExportValue(ValueFactory vf, Object value)
	    {
            StructValue sv = new StructValue(type, vf);
		    sv.Add( field, ((URL) value).ToString() );
		    return sv;
	    }
    }
}
