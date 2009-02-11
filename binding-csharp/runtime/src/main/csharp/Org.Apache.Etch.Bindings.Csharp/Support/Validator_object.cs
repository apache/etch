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
    public class Validator_object : TypeValidator
    {

        private static Validator_object[] validators = new Validator_object[MAX_CACHED];

        public override Validator ElementValidator()
        {
            return nDims > 0 ? Get(nDims - 1) : this;
        }

        private Validator_object(int nDims)
            : base(typeof (object), typeof (object), nDims, string.Format("object[{0}]", nDims), true)
        {
            
        }

        public static Validator_object Get( int nDims )
        {
            CheckDims(nDims);

            if (nDims >= validators.Length)
                return new Validator_object(nDims);

            Validator_object v = validators[nDims];

            if ( v == null )
                v=validators[nDims]= new Validator_object(nDims);

            return v;
        }      
    }
}
