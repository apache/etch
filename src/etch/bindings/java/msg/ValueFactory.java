/* $Id$
 *
 * Created by sccomer on Jan 22, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import java.util.Set;

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
	 * Translates a type id into the appropriate Type
	 * object.
	 * @param id a type id.
	 * @return id translated into the appropriate Type.
	 */
	public Type getType( Integer id );

	/**
	 * Translates a type name into the appropriate Type
	 * object.
	 * @param name a type name.
	 * @return name translated into the appropriate Type.
	 */
	public Type getType( String name );
	
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
}
