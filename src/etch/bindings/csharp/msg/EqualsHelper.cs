/// $Id$
/// Created by Champakesan, Badri Narayanan on Jun 11, 2007.
/// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
        
using System;

namespace Etch.Msg
{
        /// <param name="a"></param>
        /// <param name="b"></param>
        /// <returns>true if the value of a and b are equal even though the format is not 
        /// (e.g., 23 as a Byte vs. Integer).</returns>
        public delegate bool Equals(Object a, Object b);
}