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
    /// Validator for Boolean
    /// </summary>
    public class Validator_boolean : TypeValidator
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_boolean Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_boolean( nDims );

            Validator_boolean v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_boolean( nDims );

            return v;
        }

        private static Validator_boolean[] validators = new Validator_boolean[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_boolean( int nDims )
            : base( typeof( bool ), typeof( bool ), nDims, "boolean["+nDims+"]" )
        { 
        }


        public override Validator ElementValidator()
        {
            return Get( nDims - 1 );
        }

       
    }
}
