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
    /// Validator for long
    /// </summary>
    public class Validator_long : TypeValidator
    {
        /// <summary></summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_long Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_long( nDims );

            Validator_long v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_long( nDims );

            return v;
        }

        private static Validator_long[] validators = new Validator_long[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_long( int nDims )
            : base( typeof( long ), typeof( long ), nDims, "long["+nDims+"]" )
        { 
        }

        public override Validator ElementValidator()
        {
            return Get( nDims - 1 );
        }

        
	    public override bool Validate( object value )
	    {
		    if (nDims > 0)
			    return base.Validate( value );
		
		    if (value == null)
			    return false;
		
		    Type type = value.GetType();
		
		    return type == typeof(long) || type == typeof(Int32) ||
			type == typeof(short) || type == typeof(sbyte);
	    }
	
	   
	    public override object ValidateValue( object value )
	    {
		    value = base.ValidateValue( value );
		
		    if (value.GetType() == typeof(long) || nDims > 0)
			    return value;

            return Convert.ToInt64(value);
	    }

      
    }
}
