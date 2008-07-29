///$Id$ 
///Created by Champakesan, Badri Narayanan on Jun 13, 2007. 
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using Etch.Msg;

namespace Etch.Transport
{
    /// <summary>
    /// A TaggedDataOutputStream writes type tagged data values to an output stream.
    /// </summary>
    public interface TaggedDataOutput
    {
       
        /// <summary>
        /// Writes the message to the buf.
        /// </summary>
        /// <param name="msg">the message to be written</param>
        void WriteMessage(Message msg, Etch.Transport.FlexBuffer msgBuf);

        
    }
}