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
        /// Sets the buffer to use for future input operations.
        /// </summary>
        /// <param name="buf"></param>
        void SetBuffer( Etch.Transport.FlexBuffer buf );
        
        /// <summary>
        /// Reads a message from the buf.
        /// </summary>
        /// <returns>a message read from the buf.</returns>
        Message ReadMessage();

        /// <summary>
        /// Reads a struct from the buf.
        /// </summary>
        /// <returns>a struct read from the buf.</returns>
        StructValue ReadStruct();

        /// <summary>
        /// Reads an array from the buf.
        /// </summary>
        /// <param name="v">the validator of the values.</param>
        /// <returns>an array read from the buf.</returns>
        ArrayValue ReadArray( Validator v );
    }
    //public class StructElement 
    //{
    //    /// <summary>
    //    /// Description of key.
    //    /// </summary>
    //    public Field key;

    //    /// <summary>
    //    /// Description of value.
    //    /// </summary>
    //    public object value;
    //}

    ///// <summary>
    ///// Holder for a value from an array element sequence.
    ///// </summary>
    //public class ArrayElement 
    //{
    //    /// <summary>
    //    /// The array element value.
    //    /// </summary>
    //    public object value;
    //}
}