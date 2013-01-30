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

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * Type denotes the type of a struct or message. When used with a
 * message it typically denotes an action or event.
 */
public final class Type extends IdName
{
	/**
	 * Constructs the Type.
	 * @param id the id of the type.
	 * @param name the name of the type.
	 */
	public Type( int id, String name )
	{
		super( id, name );
	}

	/**
	 * Constructs the Type, computing the appropriate value
	 * for the id.
	 * @param name the name of the type.
	 */
	public Type( String name )
	{
		super( name );
	}

	/**
	 * @param key
	 * @return the validator for this key of this type.
	 */
	public Validator getValidator( Field key )
    {
	    return validators.get( key );
    }
	
	/**
	 * Adds the validator to the chain for this key.
	 * @param key
	 * @param vldtr
	 */
	public void putValidator( Field key, Validator vldtr )
	{
		checkNotLocked();
		
		if (vldtr == null)
			return;
		
		if (fields.get( key.getId() ) == null)
			addField( key );

		Validator v = validators.get( key );
		if (v != null)
			validators.put( key, new ComboValidator( v, vldtr ) );
		else
			validators.put( key, vldtr );
	}

	/**
	 * Removes the validator chain for this key.
	 * @param key
	 */
	public void clearValidator( Field key )
    {
		checkNotLocked();
	    validators.remove( key );
    }
	
	private final Map<Field, Validator> validators = new HashMap<Field, Validator>();
	
	/**
	 * @return the result message type of this message type.
	 */
	public Type getResult()
	{
		return rType;
	}

	/**
	 * Sets the result message type of this message type.
	 * @param rType
	 */
	public void setResult( Type rType )
	{
		checkNotLocked();
		this.rType = rType;
	}
	
	private Type rType;

	/**
	 * @return the associated import / export helper.
	 */
	public ImportExportHelper getImportExportHelper()
	{
		return helper;
	}
	
	/**
	 * Sets the associated import / export helper.
	 * @param helper
	 */
	public void setImportExportHelper( ImportExportHelper helper )
	{
		checkNotLocked();
		this.helper = helper;
	}
	
	private ImportExportHelper helper;

	/**
	 * @return the associated component type for an array of this type.
	 * This type is not used when de-serializing the array components,
	 * just when allocating the array itself.
	 */
	public Class<?> getComponentType()
	{
		return clss;
	}
	
	/**
	 * Sets the associated component type for an array of this type.
	 * This type is not used when de-serializing the array components,
	 * just when allocating the array itself.
	 * @param clss
	 */
	public void setComponentType( Class<?> clss )
	{
		checkNotLocked();
		this.clss = clss;
	}
	
	private Class<?> clss;

	/**
	 * @return an object to help the stub dispatch the received message.
	 */
	public Object getStubHelper()
	{
		return stubHelper;
	}
	
	/**
	 * Sets an object to help the stub dispatch the received message.
	 * @param stubHelper
	 */
	public void setStubHelper( Object stubHelper )
	{
//		checkNotLocked(); called from stub and not from value factory.
		if (this.stubHelper != null)
			throw new IllegalStateException( "this.stubHelper != null" );
		this.stubHelper = stubHelper;
	}

	private Object stubHelper;
	
	/**
	 * Locks the fields for this type.
	 */
	public void lock()
	{
		locked = true;
		fields.lock();
	}
	
	private void checkNotLocked()
	{
		if (locked)
			throw new IllegalStateException( "locked" );
	}
	
	private boolean locked;
	
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
	public Field addField( Field field )
	{
		return fields.add( field );
	}

	/**
	 * Translates a field id into the appropriate Field object.
	 * @param id a field id.
	 * @return id translated into the appropriate Field.
	 */
	public Field getField( Integer id )
	{
		return fields.get( id );
	}

	/**
	 * Translates a field name into the appropriate Field
	 * object.
	 * @param name a field name.
	 * @return name translated into the appropriate Field.
	 */
	public Field getField( String name )
	{
		return fields.get( name );
	}
	
	/**
	 * @return a set of all the fields.
	 */
	public Set<Field> getFields()
	{
		return Collections.unmodifiableSet( fields.values() );
	}
	
	private final FieldMap fields = new FieldMap();

	/**
	 * @return the time in milliseconds to wait for this response message.
	 */
	public int getTimeout()
	{
		return timeout;
	}

	/**
	 * @param timeout the time in milliseconds to wait for this response message.
	 */
	public void setTimeout( int timeout )
	{
		checkNotLocked();
		this.timeout = timeout;
	}
	
	private int timeout;

	/**
	 * @return the field with the value of this response.
	 */
	public Field getResponseField()
	{
		return responseField;
	}

	/**
	 * @param responseField the field with the value of this response.
	 */
	public void setResponseField( Field responseField )
	{
		checkNotLocked();
		this.responseField = responseField;
	}
	
	private Field responseField;
	
	/**
	 * Checks whether this type is assignment compatible with other. This
	 * means that other is a subclass of this.
	 * @param other
	 * @return true if this type is assignable from other.
	 */
	public boolean isAssignableFrom( Type other )
	{
		return other != null && (equals( other ) || isAssignableFrom( other.superType() ));
	}

	/**
	 * Checks to see if this type is assignable from the other type.
	 * @param other
	 * @throws IllegalArgumentException if this not assignable from other.
	 * @see #isAssignableFrom(Type)
	 */
	public void checkIsAssignableFrom( Type other )
	{
		if (!isAssignableFrom( other ))
			throw new IllegalArgumentException( "this not super type of other" );
	}

	/**
	 * @return the super type of this type. If struct A extends B, then
	 * B is the super type of A.
	 */
	public Type superType()
	{
		return superType;
	}
	
	/**
	 * Sets the super type of this type. If struct A extends B, then
	 * B is the super type of A.
	 * @param superType
	 */
	public void setSuperType( Type superType )
	{
		checkNotLocked();
		this.superType = superType;
	}
	
	private Type superType;

	/**
	 * @return the AsyncMode for this type.
	 */
	public AsyncMode getAsyncMode()
	{
		return asyncMode;
	}
	
	/**
	 * Sets the AsyncMode for this type.
	 * @param mode
	 */
	public void setAsyncMode( AsyncMode mode )
	{
		checkNotLocked();
		asyncMode = mode;
	}
	
	private AsyncMode asyncMode;

	/**
	 * @return the message direction.
	 */
	public Direction getDirection()
	{
		return direction;
	}

	/**
	 * Sets the message direction.
	 * @param direction the direction to set.
	 */
	public void setDirection( Direction direction )
	{
		checkNotLocked();
		this.direction = direction;
	}
	
	private Direction direction;
}
