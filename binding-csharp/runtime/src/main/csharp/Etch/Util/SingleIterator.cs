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
using System.Collections.Generic;

namespace Etch.Util
{
    /// <summary>
    /// An iterator over a single object
    /// </summary>
    public class SingleIterator<E> : IEnumerator<E>
    {
        private E _obj;

        public SingleIterator( Object obj )
        {
            _obj = ( E ) obj;
        }

        #region IEnumerator<E> Members

        public E Current
        {
            get 
            {
                if ( _obj == null )
                    throw new Exception( "No Such element" );

                E o = _obj;
                _obj = default(E);
                return o;
            }
        }

        #endregion

        #region IDisposable Members

        public void Dispose()
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        #endregion

        #region IEnumerator Members

        object System.Collections.IEnumerator.Current
        {
            get
            {
                if ( _obj == null )
                    throw new Exception( "No Such element" );

                E o = _obj;
                _obj = default( E );
                return o;
            }
        }

        public bool MoveNext()
        {
            return _obj != null;
        }

        public void Reset()
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        #endregion
    }
}
