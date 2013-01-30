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
    /// Base class of validators which validate based on exact type.
    /// </summary>
    abstract public class TypeValidator : Validator
    {
        
        /// <summary>
        /// Constructs the TypeValidator.
        /// </summary>
        /// <param name="scalarClass">class to use if nDims == 0</param>
        /// <param name="arrayClass">class to use if nDims > 0</param>
        /// <param name="nDims">the number of dimensions. 0 for a scalar.</param>
        /// <param name="descr"></param>
        public TypeValidator( Type scalarClass, Type arrayClass, int nDims,
            String descr)
            : this(scalarClass, arrayClass, nDims, descr, false)
        {
          
        }

        /// <summary>
        /// Constructs the TypeValidator.
        /// </summary>
        /// <param name="scalarClass">class to use if nDims == 0</param>
        /// <param name="arrayClass">class to use if nDims > 0</param>
        /// <param name="nDims">the number of dimensions. 0 for a scalar.</param>
        /// <param name="descr"></param>
        /// <param name="subclassOk">true of a subclass of the expected class is ok</param>
        public TypeValidator(Type scalarClass, Type arrayClass, int nDims,
            String descr, bool subclassOk)
        {
            CheckDims(nDims);

            this.expectedClass = MkExpectedClass(scalarClass, arrayClass, nDims);
            this.nDims = nDims;
            this.descr = descr;
            this.subclassOk = subclassOk;
        }

        /// <summary>
        /// Checks the number of dimensions for standard validators.
        /// </summary>
        /// <param name="nDims"></param>
        public static void CheckDims( int nDims )
        {
            if ( nDims < 0 || nDims > MAX_NDIMS )
                throw new ArgumentOutOfRangeException( "nDims < 0 || nDims > MAX_NDIMS" );
        }

        private Type expectedClass;

        /// <summary>
        /// number of dimensions if this is an array
        /// </summary>
        protected int nDims;

        protected bool subclassOk;

        public int GetNDims()
        {
            return nDims;
        }

        private String descr;

        public Type GetExpectedClass()
        {
            return expectedClass;
        }

        public override string ToString()
        {
            return descr;
        }

        public override bool Validate( object value )
        {
            if (value == null)
                return false;
            Type clss = value.GetType();
          
            if (clss == expectedClass)
                return true;
     
            return subclassOk && expectedClass.IsAssignableFrom(clss);
        }

        public override object ValidateValue(object value)
        {
            if (Validate(value))
                return value;

            throw new Exception("value not appropriate for " + descr + ": " + value);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="scalarClass">class to use if nDims == 0</param>
        /// <param name="arrayClass">class to use if nDims > 0</param>
        /// <param name="nDims">number of dimensions, 0 for scalar</param>
        /// <returns>an appropriate class given nDims.</returns>
        /// 
        private static Type MkExpectedClass( Type scalarClass, Type arrayClass, int nDims )
        {
            if ( nDims == 0 )
                return scalarClass;

            Array tempArray = Array.CreateInstance( arrayClass, 1 );
            // Note: About C# jagged arrays
            // 
            for ( int i = 0; i < ( nDims - 1 ); i++ )
                tempArray =  Array.CreateInstance( tempArray.GetType(), 1 );

            return tempArray.GetType();
        }
    }
}
