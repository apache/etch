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
    public class Class2TypeMap
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="c"></param>
        /// <returns>the Type for the specified class, or null</returns>
        public XType Get( Type c )
        {
            try
            {
                return c2t[ c ];
            }
            catch ( KeyNotFoundException )
            {
                return null;
            }
        }

        /// <summary>
        /// Adds a map entry from c to t
        /// </summary>
        /// <param name="c"></param>
        /// <param name="t"></param>
        public void Add( Type c, XType t )
        {
            if ( locked )
                throw new Exception( "locked" );

            XType x;
            try
            {
                x = c2t[ c ];
            }
            catch ( KeyNotFoundException )
            {
                x = null;
            }

            if ( x != null )
            {
                if ( !x.Equals( t ) )
                    throw new ArgumentException( String.Format( "type {0} : class {1} is already mapped to type {2}", t, c, x ) );
                return;
            }
            c2t.Add( c, t );
        }

        /// <summary>
        /// Adds all the mappings from other to this
        /// </summary>
        /// <param name="other"></param>
        public void AddAll( Class2TypeMap other )
        {
            if ( locked )
                throw new Exception( "locked" );

            foreach ( KeyValuePair<Type, XType> me in other.c2t )
            {
                Add( me.Key, me.Value );
            }
        }

        /// <summary>
        /// Locks the map, disallowing any more changes
        /// </summary>
        public void Lock()
        {
            locked = true;
        }

        private readonly Dictionary<Type, XType> c2t = new Dictionary<Type, XType>();

        private Boolean locked;
    }
}
