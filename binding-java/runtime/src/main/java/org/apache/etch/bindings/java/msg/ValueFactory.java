/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.bindings.java.msg;

import java.util.Set;

import org.apache.etch.bindings.java.msg.Validator.Level;


/**
 * Interface which defines the value factory which helps
 * the idl compiler serialize and deserialize messages,
 * convert values, etc.
 */
public interface ValueFactory
{
	//////////
	// Type //
	//////////
	
	/**
	 * Translates a type id into the appropriate Type object. If the type does
	 * not exist, and if dynamic typing is enabled, adds it to the dynamic types.
	 * @param id a type id.
	 * @return id translated into the appropriate Type.
	 */
	public Type getType( Integer id );

	/**
	 * Translates a type name into the appropriate Type object. If the type does
	 * not exist, and if dynamic typing is enabled, adds it to the dynamic types.
	 * @param name a type name.
	 * @return name translated into the appropriate Type.
	 */
	public Type getType( String name );

	/**
	 * Adds the type if it doesn't already exist. Use this to dynamically add
	 * types to a ValueFactory. The type is per instance of the ValueFactory,
	 * not global. Not available if dynamic typing is locked.
	 * @param type
	 */
	public void addType( Type type );

	/**
	 * Locks the dynamic typing so that no new types may be created by addType
	 * or getType.
	 */
	public void lockDynamicTypes();

	/**
	 * Unlocks the dynamic typing so that new types may be created by addType
	 * or getType.
	 */
	public void unlockDynamicTypes();
	
	/**
	 * @return a collection of all the types.
	 */
	public Set<Type> getTypes();

	/////////////////////
	// STRING ENCODING //
	/////////////////////

	/**
	 * @return the encoding to use for strings.
	 */
	public String getStringEncoding();

	////////////////
	// MESSAGE ID //
	////////////////

	/**
	 * @param msg the message whose well-known message-id field is to be
	 * returned.
	 * @return the value of the well-known message-id field. This is a
	 * unique identifier for this message on a particular transport
	 * during a particular session. If there is no well-known message-id
	 * field defined, or if the message-id field has not been set, then
	 * return null.
	 */
	public Long getMessageId( Message msg );

	/**
	 * Sets the value of the well-known message-id field. This is a
	 * unique identifier for this message on a particular transport
	 * during a particular session. If there is no well-known message-id
	 * field defined then nothing is done. If msgid is null, then the
	 * field is cleared.
	 * @param msg the message whose well-known message-id field is to
	 * be set.
	 * @param msgid the value of the well-known message-id field.
	 */
	public void setMessageId( Message msg, Long msgid );

	/**
	 * @return well-known message field for message id.
	 */
	public Field get_mf__messageId();

	/////////////////
	// IN REPLY TO //
	/////////////////
	
	/**
	 * @param msg the message whose well-known in-reply-to field is to
	 * be returned.
	 * @return the value of the in-reply-to field, or null if there is
	 * none or if there is no such field defined.
	 */
	public Long getInReplyTo( Message msg );

	/**
	 * @param msg the message whose well-known in-reply-to field is to
	 * be set.
	 * @param msgid the value of the well-known in-reply-to field. If
	 * there is no well-known in-reply-to field defined then nothing
	 * is done. If msgid is null, then the field is cleared.
	 */
	public void setInReplyTo( Message msg, Long msgid );

	/**
	 * @return well-known message field for in reply to.
	 */
	public Field get_mf__inReplyTo();

	//////////////////////
	// VALUE CONVERSION //
	//////////////////////

	/**
	 * Converts a value to a struct value representation to be exported
	 * to a tagged data output.
	 * @param value a custom type defined by a service, or a well-known
	 * standard type (e.g., date).
	 * @return a struct value representing the value.
	 * @throws UnsupportedOperationException if the type cannot be exported.
	 */
	public StructValue exportCustomValue( Object value )
		throws UnsupportedOperationException;
	
	/**
	 * Converts a struct value imported from a tagged data input to
	 * a normal type.
	 * @param struct a struct value representation of a custom type, or a
	 * well known standard type.
	 * @return a custom type, or a well known standard type.
	 * @throws UnsupportedOperationException if the type cannot be imported.
	 */
	public Object importCustomValue( StructValue struct )
		throws UnsupportedOperationException;

	/**
	 * @param c the class of a custom value.
	 * @return the struct type of a custom value class.
	 * @throws UnsupportedOperationException
	 * @see #exportCustomValue(Object)
	 */
	public Type getCustomStructType( Class<?> c )
		throws UnsupportedOperationException;

	/**
	 * @return well-known message type for exception thrown by one-way
	 * message.
	 */
	public Type get_mt__exception();

	/**
	 * @return the validation level of field StructValue.put and TaggedDataOutput.
	 */
	public Level getLevel();

	/**
	 * Sets the validation level of field StructValue.put and TaggedDataOutput.
	 * @param level
	 * @return the old value
	 */
	public Level setLevel( Level level );
}
