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

namespace Org.Apache.Etch.Bindings.Csharp.Msg
{
    /// <summary>
    /// Validate values put into messages
    /// </summary>
    abstract public class Validator
    {
        /// <summary>
        /// The validation level.
        /// </summary>
        public enum Level
        {
            /// <summary>
            /// No validation on put or output.
            /// </summary>
            NONE,
            /// <summary>
            /// Validation on put or output if validator defined.
            /// </summary>
            MISSING_OK,
            /// <summary>
            /// Like MISSING_OK, but fail if no validator defined.
            /// </summary>
            FULL
        }

        /// <summary>
        /// The maximum number of dimensions for arrays
        /// </summary>
        public const int MAX_NDIMS = 9;

        /// <summary>
        /// The maximum number of validators to cache per type.
        /// </summary>
        protected const int MAX_CACHED = 4;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>for an array type, return a validator for an element of
        /// the array (which might be a scalar or another array element).</returns>
        abstract public Validator ElementValidator();

        /// <summary>
        /// Checks the value for being valid. If the value is valid
        /// by this validator, return true.
        /// </summary>
        /// <param name="value">the value to be validated.</param>
        /// <returns>true if the value is valid by any validator in the
        /// chain, or false if all reject it.</returns>
        abstract public bool Validate( Object value );

       

        /// <summary>
        /// </summary>
        /// <param name="value"></param>
	    /// <returns> return the appropriate value given the input value and this
	    /// validator's sensibility about what it should be really (e.g.,
	    /// input Byte but output Long. </returns>
	 
        abstract public Object ValidateValue(Object value);
    }
}
