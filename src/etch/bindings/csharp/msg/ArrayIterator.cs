///$Id$
///Created by Champakesan, Badri Narayanan on Jun 14, 2007. 
/// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
  
using System;
using System.Collections.Generic;

namespace Etch.Msg
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
