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
using System.Runtime.CompilerServices;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Synchronously generates long id numbers. 
    /// </summary>
    public sealed class IdGenerator 
    {
        /// <summary>
        /// Constructs the IdGenerator with the default starting point of 1
        /// and the default stride of 1.
        /// </summary>
        public IdGenerator() : this( 1, 1 )
        {
            
        }

        /// <summary>
        /// Constructs the IdGenerator with the specified starting point
        /// and the default stride of 1.
        /// </summary>
        /// <param name="nextId"></param>
        public IdGenerator( long nextId ) : this( nextId, 1 )
        {
            
        }

        /// <summary>
        /// Constructs the IdGenerator with the specified starting point
        /// and the specified stride.
        /// </summary>
        /// <param name="nextId"></param>
        /// <param name="stride"></param>
        public IdGenerator( long nextId, int stride )
        {
            if ( stride <= 0 )
                throw new ArgumentException( "stride <= 0" );

            this.nextId = nextId;
            this.stride = stride;
        }

        private long nextId;
	
	    private readonly int stride;

        /// <summary>
        /// 
        /// </summary>
        /// <returns>The next Id in sequence</returns>
        [MethodImpl(MethodImplOptions.Synchronized)]
        public long Next()
        {
            long id = nextId;
            nextId += stride;
            return id;
        }

    }
}
