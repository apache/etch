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
using System.Collections.Generic;
using Org.Apache.Etch.Bindings.Csharp.Msg;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    /// <summary>
    /// Validator for custom type
    /// </summary>
    public class Validator_custom : TypeValidator
    {
        /// <summary></summary>
        /// <param name="clss">Class of the custom type</param>
        /// <param name="nDims">number of dimensions. 0 for scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator Get( Type clss, int nDims, bool subclassOk )
        {
            CheckDims( nDims );

            if ( nDims > MAX_CACHED )
                return new Validator_custom( clss, nDims, subclassOk );

            Key key = new Key(clss, nDims, subclassOk);
            Validator v = null;
            
            try
            {
                 v = validators[key];
            }
            catch(Exception)
            {
                v = null;
            }

            if (v == null)
            {
                v = new Validator_custom(clss, nDims, subclassOk);
                validators.Add(key, v);
            }

            return v;

        }


        private  class Key
	    {
		    public Key( Type clss, int dims, bool subclassOk )
		    {
			    this.clss = clss;
			    this.dims = dims;
			    this.subclassOk = subclassOk;
		    }
		
		    private Type clss;
		
		    private int dims;
		
		    private bool subclassOk;
		
		
		    public override int GetHashCode()
		    {
			    return clss.GetHashCode() ^ (dims * 9131) ^ (subclassOk ? 21357 : 8547);
		    }
		
	
		    public override bool Equals( Object obj )
		    {
			    if (obj == this)
				    return true;
			
			    if (obj == null)
				    return false;
			
			    if (obj.GetType() != GetType())
				    return false;
			
			    Key other = (Key) obj;
			
			    return other.clss.Equals( clss)
				    && other.dims == dims
				    && other.subclassOk == subclassOk;
		    }
	    }

        private static Dictionary<Key, Validator> validators = 
            new Dictionary<Key, Validator>();

        /// <summary>
        /// Constructs the validator
        /// </summary>
        /// <param name="clss">the class of the custom type</param>
        /// <param name="nDims">the number of dimensions. 0 for scalar. </param>
        private Validator_custom( Type clss, int nDims, bool subclassOk )
            : base(clss, clss, nDims, subclassOk ? clss.ToString() + "[" + nDims + "]*" : clss.ToString() + "[" + nDims + "]", subclassOk)
        {
            this.clss = clss;
        }

        private Type clss;

        public override Validator ElementValidator()
        {
            return Get( clss, nDims - 1, subclassOk );
        }

        
    }
}
