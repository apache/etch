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
using Org.Apache.Etch.Bindings.Csharp.Msg;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    /// <summary>
    /// Validator for structvalue
    /// </summary>
    public class Validator_StructValue : TypeValidator
    {
        public static Validator_StructValue Get(XType type, int nDims)
        {
            CheckDims(nDims);          
            return new Validator_StructValue(type,nDims);
        }

       

        private Validator_StructValue(XType type, int nDims)
            : base( typeof( StructValue ), typeof( StructValue ), nDims, string.Format("StructValue[{0},{1}]",type,nDims) )
        {
            this.type = type;
        }

        private XType type;

        public XType GetXType()
        {
            return type;
        }

        public override Validator ElementValidator()
        {
            return Get(type, nDims - 1);
        }

        public override bool Validate( object value )
        {
            return
                base.Validate(value) &&
                (value.GetType() != typeof (StructValue) || type.IsAssignableFrom(((StructValue) value).GetXType));
 
        }     
    }
}
