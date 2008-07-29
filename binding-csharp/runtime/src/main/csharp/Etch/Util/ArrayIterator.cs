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

using System.Collections.Generic;

namespace Etch.Util
{
    public class ArrayIterator<T> : IEnumerator<T> 
    {

        /// <param name="values"></param>
        public ArrayIterator( T[] values ) 
       {
            this.values = values;
       }

        private T[] values;

        private int index = -1;

        /// <returns> index value if less than the length of values</returns>
        public bool MoveNext()
        {
            if(index < values.Length)
                index++;
            return index < values.Length;
        }

        /// <summary>Resets index before the beginning of the array</summary>
        public void Reset()
        {
            index = -1;
        }

        /// <summary>returns the value at the index of the 'values'</summary>
        public T Current
        {
            get
            {
                return values[index];
            }
        }

        /// <summary>returns the value at the index of the 'values'</summary>
        object System.Collections.IEnumerator.Current
        {
            get
            {
                return values[index];
            }
        }

        public void Dispose(){}
    }
}
