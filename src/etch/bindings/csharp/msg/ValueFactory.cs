///$Id$
///Created by Champakesan, Badri Narayanan on Jun 13, 2007.
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{ 
    /// <summary>Interface which defines the value factory which helps
    ///  the idl compiler serialize and deserialize messages, convert values, etc.</summary>
    public interface ValueFactory 
    {
        //////////
        // XType //
        //////////

        /// <summary>Translates a type id into the appropriate XType object.</summary>
        ///<param> id a type id.</param>
        /// <returns> id translated into the appropriate XType.</returns>  
        XType GetType(int? id);

        /// <summary>
        /// Translates a type name into the appropriate XType object.
        /// </summary>
        /// <param>name a type name.</param>
        /// <returns> name translated into the appropriate XType.</returns>
        
        XType GetType(string name);

        /// <returns> a collection of all the types.</returns>
        ICollection<XType> GetTypes();

        /////////////////////
        // STRING ENCODING //
        /////////////////////

        ///<returns> the encoding to use for strings.
        Encoding GetStringEncoding();

        ////////////////
        // MESSAGE ID //
        ////////////////

        /// <param name="msg">msg the message whose well-known message-id field is to be returned.</param>
        /// <returns>the value of the well-known message-id field. This is a unique identifier 
        /// for this message on a particular transport during a particular session. If there is no well-known 
        /// message-id field defined, or if the message-id field has not been set, then return null.</returns>

        long? GetMessageId(Message msg);
        
        /// <summary> Sets the value of the well-known message-id field. This is a unique identifier 
        /// for this message on a particular transport during a particular session. If there is no 
        /// well-known message-id field defined then nothing is done. If msgid is null, then the field is cleared.
        /// </summary>
        /// <param>msg the message whose well-known message-id field is to be set.</param> 
        /// <param>msgid the value of the well-known message-id field.</param> 
        
        void SetMessageId(Message msg, long? msgid); 
        
        /////////////////
        // IN REPLY TO //
        /////////////////

        /// <returns>the value of the in-reply-to field, or null if there is none or if there is no such field defined.
        /// </returns>
        ///<param> msg the message whose well-known in-reply-to field is to be returned.</param>

        long? GetInReplyTo(Message msg);

        /// <param>msg the message whose well-known in-reply-to field is to be set.</param>
        /// <param>msgid the value of the well-known in-reply-to field. If there is no well-known 
        /// in-reply-to field defined then nothing is done. If msgid is null, then the field is cleared.</param>
        /// </summary> 
        
        void SetInReplyTo(Message msg, long? msgid);

        //////////////////////
        // VALUE CONVERSION //
        //////////////////////

        /// <summary>Converts a value to a struct value representation to be exported to a tagged data output.</summary>
        /// <param>value a custom type defined by a service, or a well-known standard type (e.g., date).</param> 
        /// <returns> a struct value representing the value.</returns>
        /// <throws>UnsupportedOperationException if the type cannot be exported.</throws> 
        
        StructValue ExportCustomValue(object value);

        /// <summary>Converts a struct value imported from a tagged data input to a normal type.</summary>
        /// <param>sv a struct value representation of a custom type, or a well known standard type.</param> 
        /// <returns> a custom type, or a well known standard type.</returns>
        /// <throws> UnsupportedOperationException if the type cannot be imported.</throws>
        
        Object ImportCustomValue(StructValue sv);

        /// <summary>
        /// 
        /// </summary>
        /// <param name="c">the class of a custom value.</param>
        /// <returns>the struct type of a custom value class.</returns>
        XType GetCustomStructType( Type c );


        ///<summary></summary>
        /// <returns>well-known message type for exception thrown by one-way message</returns>	
        XType get_mt__exception();

        
    }
}