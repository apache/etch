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

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    /// <summary>
    /// Validator for Runtime Exceptions
    /// </summary>
    public class Validator_RuntimeException : Validator
    {
        /// <summary></summary>
        /// <returns>an instance of the validator</returns>
        public static Validator_RuntimeException Get()
        {
            if ( validator == null )
                validator = new Validator_RuntimeException();

            return validator;
        }

        private static Validator_RuntimeException validator;

        public Validator_RuntimeException()
        {
            // nothing to do
        }

        public override Validator ElementValidator()
        {
            throw new Exception( "not an array" );
        }

        public override bool Validate( object value )
        {
            return ( value is Exception );
        }

        public override object ValidateValue(object value)
        {
            if (Validate(value))
                return value;

            throw new Exception("value not appropriate for RuntimeException: " + value);
        }      
    }
}
