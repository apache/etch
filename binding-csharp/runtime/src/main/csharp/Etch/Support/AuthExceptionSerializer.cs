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

namespace Etch.Support
{
    public class AuthExceptionSerializer : ImportExportHelper
    {
        private const String FIELD_NAME = "msg";

        public static void Init(XType type, Class2TypeMap class2type)
        {
            Field field = type.GetField(FIELD_NAME);
            class2type.Add(typeof(_Etch_AuthException), type);
            type.SetComponentType(typeof(_Etch_AuthException));
            type.SetImportExportHelper(new AuthExceptionSerializer(type, field));
            type.PutValidator(field, Validator_string.Get(0));
            type.Lock();
        }

        public AuthExceptionSerializer(XType type, Field field)
        {
            this.type = type;
            this.field = field;
        }

        private readonly XType type;

        private readonly Field field;

        public override StructValue ExportValue(ValueFactory vf, Object value)
        {
            StructValue sv = new StructValue(type, vf);
            sv.Add(field, value.ToString());
            return sv;
        }

        public override Object ImportValue(StructValue sv)
        {
            sv.CheckType(type);
            return new _Etch_AuthException((String)sv.Get(field));
        }
    }
}
