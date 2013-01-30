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
using System.Runtime.CompilerServices;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    ///  A safe integer counter
    /// </summary>
    public class IntCounter
    {
        /// <summary>
        /// Adjusts the value of the counter by k
        /// </summary>
        /// <param name="k">the adjustment to the counter.</param>
        /// <returns>the new value of the counter.</returns>
        /// 
        [MethodImpl( MethodImplOptions.Synchronized )]
        public int Adjust( int k )
        {
            value += k;
            return value;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the value of the counter</returns>
        public int Get()
        {
            return value;
        }

        private int value;
    }
}
