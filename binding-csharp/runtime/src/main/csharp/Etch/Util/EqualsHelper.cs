// $Id$
// 
// Copyright 2007-2008 Cisco Systems Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using System;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns>true if the value of a and b are equal even though the format is not 
        /// (e.g., 23 as a Byte vs. Integer).</returns>
        public delegate bool Equals(Object a, Object b);
}