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
        /// Sets the buffer for future output operations
        /// </summary>
        /// <param name="msgBuf"></param>
        void SetBuffer( Etch.Transport.FlexBuffer msgBuf );

        /// <summary>
        /// Writes the message to the buf.
        /// </summary>
        /// <param name="msg">the message to be written</param>
        void WriteMessage( Message msg );

        /// <summary>
        /// Writes the struct to the buf.
        /// </summary>
        /// <param name="sv">the struct to be written</param>
        void WriteStruct( StructValue sv );

        /// <summary>
        /// Writes the array to the buf.
        /// </summary>
        /// <param name="av">the array to be written.</param>
        /// <param name="v">the validator of the values.</param>
        void WriteArray( ArrayValue av, Validator v );
    }
}