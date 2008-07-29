/// $Id$
/// Created by Champakesan, Badri Narayanan on Jun 8, 2007.
/// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
using System;
using Etch.Msg;

namespace Etch.Transport
{
    /// <summary>A TaggedDataInputStream reads type tagged values from an input stream.</summary>
    public interface TaggedDataInput
    {
       
        
        /// <summary>
        /// Reads a message from the buf.
        /// </summary>
        /// <returns>a message read from the buf.</returns>
        Message ReadMessage(Etch.Transport.FlexBuffer buf);

      
    }
}