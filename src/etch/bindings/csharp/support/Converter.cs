///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 22, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using Etch.Msg;

namespace Etch.Support
{
  /// <summary>Interface to help with type conversions of array elements. An example is the return value of a 
  /// method is to be an array of Integer but the actual value is an array of various Byte, Short and Integer. 
  /// The Bytes and Shorts would be converted to Integers.</summary>
    public interface Converter
    {
        /// <summary></summary>
        /// <param name="obj"></param>
        /// <returns>the object converted to a conformant type.</returns>
        
        Object Convert( Object obj);
    }
}