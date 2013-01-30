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

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    public class CharIterator 
    {
        private readonly String _s;
        private readonly int _n;
        private int _i = -1;

        public CharIterator( String s )
        {
            if ( s == null )
                throw new ArgumentNullException( "String to iterate over shouldn't be null" );

            _s = s;
            _n = s.Length;
        }

        public Boolean MoveNext()
        {
            if ( _i < (_n-1) )
            {
                _i++;
                return true;
            }
            else
                return false;
        }

        public Char Current
        {
            get
            {
                return _s[ _i ];
            }
        }
    }
}
