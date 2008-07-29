/* $Id$
 *
 * Created by sccomer on Jan 22, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import java.util.Collection;

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
	 * Adds a type to the set of types.
	 * 
	 * @param type a type to add.
	 * 
	 * @return the argument. If there is a collision with
	 * an id and name, both associated with the same type,
	 * then that type is returned instead of the argument.
	 * 
	 * @throws IllegalArgumentException if there is a
	 * collision in the id or name, or both id and name
	 * when not associated with the same type.
	 */
	public Type addType( Type type );
	
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
	public Collection<Type> getTypes();

	///////////
	// Field //
	///////////
	
	/**
	 * Adds a field to the set of fields.
	 * 
	 * @param field a field to add.
	 * 
	 * @return the argument. If there is a collision with
	 * an id and name, both associated with the same field,
	 * then that field is returned instead of the argument.
	 * 
	 * @throws IllegalArgumentException if there is a
	 * collision in the id or name, or both id and name
	 * when not associated with the same field.
	 */
	public Field addField( Field field );

	/**
	 * Translates a field id into the appropriate Field object.
	 * @param id a field id.
	 * @return id translated into the appropriate Field.
	 */
	public Field getField( Integer id );

	/**
	 * Translates a field name into the appropriate Field
	 * object.
	 * @param name a field name.
	 * @return name translated into the appropriate Field.
	 */
	public Field getField( String name );
	
	/**
	 * @return a collection of all the fields.
	 */
	public Collection<Field> getFields();

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
	 * @param sv a struct value representation of a custom type, or a
	 * well known standard type.
	 * @return a custom type, or a well known standard type.
	 * @throws UnsupportedOperationException if the type cannot be imported.
	 */
	public Object importCustomValue( StructValue sv )
		throws UnsupportedOperationException;

	/**
	 * @param c the class of a custom value.
	 * @return the struct type of a custom value class.
	 * @throws UnsupportedOperationException 
	 * @see #exportCustomValue(Object)
	 * @see #getCustomType(Type)
	 */
	public Type getCustomStructType( Class<?> c )
		throws UnsupportedOperationException;

	/**
	 * @param type the type of a custom value
	 * @return the class of a custom struct value type.
	 * @throws UnsupportedOperationException
	 * @see #importCustomValue(StructValue)
	 * @see #getCustomStructType(Class)
	 */
	public Class<?> getCustomType( Type type )
		throws UnsupportedOperationException;
}
