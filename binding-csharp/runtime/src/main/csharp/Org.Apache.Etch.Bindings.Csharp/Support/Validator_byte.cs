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
    /// Validator for byte
    /// </summary>
    public class Validator_byte : TypeValidator
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="nDims">number of dimensions. 0 for scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_byte Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_byte( nDims );

            Validator_byte v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_byte( nDims );

            return v;
        }

        private static Validator_byte[] validators = new Validator_byte[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_byte( int nDims )
            : base( typeof( sbyte ), typeof( sbyte ), nDims, "byte["+nDims+"]" )
        {
        }

        public override Validator ElementValidator()
        {
            return Get( nDims - 1 );
        }
      	    
	    public override bool Validate( object value )
	    {
		    if (nDims > 0)
			   return base.Validate(value);
		
		    if (value == null)
			    return false;

            if (value.GetType() == typeof(sbyte))
			    return true;
		
		    if (value.GetType() == typeof(short))
		    {
			    short v = (short) value;
			    return v >= SByte.MinValue && v <= SByte.MaxValue;
               
		    }
		
		   if (value.GetType() == typeof(Int32))
		    {
			    int v = (Int32) value;
			    return v >= SByte.MinValue && v <= SByte.MaxValue;
		    }
		
		   if (value.GetType() == typeof(long))
		   {
			    long v = (long) value;
			    return v >= SByte.MinValue && v <= SByte.MaxValue;
		    }
		
		    return false;
	    }
		
	    public override object ValidateValue( object value )
	    {
		    value = base.ValidateValue(value);
		
		    if ((value.GetType() == typeof(sbyte)) || nDims > 0)
			    return value;

            return Convert.ToSByte(value);
	    }      
    }
}
