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
    /// Validates values using one of two validators
    /// </summary>
    public class ComboValidator : Validator
    {
        /// <summary>
        /// Constructs the combo validator
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        public ComboValidator( Validator a, Validator b )
        {
            this.a = a;
            this.b = b;
        }

        private readonly Validator a;
        private readonly Validator b;

        public override string ToString()
        {
            return String.Format( "({0} OR {1})", a, b );
        }

        public override Validator ElementValidator()
        {
            Validator na;

            try
            {
                na = a.ElementValidator();
            }
            catch ( Exception )
            {
                na = null;
            }

            Validator nb;
            try
            {
                nb = b.ElementValidator();
            }
            catch ( Exception )
            {
                nb = null;
            }

            if ( na == null && nb == null )
                throw new ArgumentException( "na == null and nb == null" );

            if ( na == null )
                return nb;

            if ( nb == null )
                return na;

            return new ComboValidator( na, nb );

        }

        public override bool Validate( object value )
        {
            return a.Validate( value ) || b.Validate( value );
        }

      

        public override object ValidateValue(object value)
        {
            try
            {
                return a.ValidateValue(value);
            }
            catch (Exception)
            {
                return b.ValidateValue(value);
            }
        }

        public Validator A()
        {
            return a;
        }

        public Validator B()
        {
            return b;
        }
    }
}
