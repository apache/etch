// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using System.Collections.Generic;
using System.Text;

namespace Org.Apache.Etch.Bindings.Csharp.Msg
{ 
    /// <summary>Interface which defines the value factory which helps
    ///  the idl compiler serialize and deserialize messages, convert values, etc.</summary>
    public interface ValueFactory 
    {
        //////////
        // XType //
        //////////

        /// <summary>Translates a type id into the appropriate XType object.
        /// If the type does not exist, and if dynamic typing is enabled,
        /// adds it to the dynamic types.</summary>
        ///<param> id a type id.</param>
        /// <returns> id translated into the appropriate XType.</returns>  
        XType GetType(int id);

        /// <summary>Translates a type name into the appropriate XType object.
        /// If the type does not exist, and if dynamic typing is enabled,
        /// adds it to the dynamic types.</summary>
        /// <param>name a type name.</param>
        /// <returns> name translated into the appropriate XType.</returns>
        
        XType GetType(string name);

        /// <summary>
        /// Adds the type if it doesn't already exist. Use this to dynamically add
        /// types to a ValueFactory. The type is per instance of the ValueFactory,
        /// not global. Not available if dynamic typing is locked.
        /// </summary>
        /// <param name="type"></param>
        void AddType(XType type);

        /// <summary>
        /// Locks the dynamic typing so that no new types may be created by addType
        /// or getType.
        /// </summary>
        void LockDynamicTypes();

        /// <summary>
        /// Unlocks the dynamic typing so that new types may be created by addType
        /// or getType.
        /// </summary>
        void UnlockDynamicTypes();

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

        /// <summary>
        /// Gets well-known message field for message id.
        /// </summary>
        /// <returns>well-known message field for message id.</returns>
        Field Get_mf__messageId();

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

        /// <summary>
        /// Gets well-known message field for in reply to.
        /// </summary>
        /// <returns>well-known message field for in reply to.</returns>
        Field Get_mf__inReplyTo();

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

        /// <summary>
        /// Gets the put / write validation level.
        /// </summary>
        /// <returns>the put / write validation level</returns>
        Validator.Level GetLevel();

        /// <summary>
        /// Sets the put / write validation level.
        /// </summary>
        /// <param name="level">the new put / write validation level</param>
        /// <returns>the old put / write validation level</returns>
        Validator.Level SetLevel(Validator.Level level);
    }
}