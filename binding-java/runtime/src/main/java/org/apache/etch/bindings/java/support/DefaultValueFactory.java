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

package org.apache.etch.bindings.java.support;

import java.util.List;
import java.util.Map;
import java.util.Set;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.ImportExportHelper;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.msg.Validator.Level;
import org.apache.etch.bindings.java.util.DateSerializer;
import org.apache.etch.bindings.java.util.ListSerializer;
import org.apache.etch.bindings.java.util.MapSerializer;
import org.apache.etch.bindings.java.util.SetSerializer;
import org.apache.etch.util.URL;


/**
 * Default implementation of ValueFactory which provides some
 * dynamic type and field support, as well as standard
 * value conversions and import and rt.
 */
public class DefaultValueFactory implements ValueFactory
{
	private static final String ETCH_RUNTIME_EXCEPTION_TYPE_NAME = "_Etch_RuntimeException";

	private static final String ETCH_LIST_TYPE_NAME = "_Etch_List";

	private static final String ETCH_MAP_TYPE_NAME = "_Etch_Map";

	private static final String ETCH_SET_TYPE_NAME = "_Etch_Set";

	private static final String ETCH_DATETIME_TYPE_NAME = "_Etch_Datetime";

	private static final String ETCH_AUTH_EXCEPTION_TYPE_NAME = "_Etch_AuthException";

	private static final String ETCH_EXCEPTION_MESSAGE_NAME = "_exception";

	private static final String MSG_FIELD_NAME = "msg";

	private static final String MESSAGE_ID_FIELD_NAME = "_messageId";

	private static final String IN_REPLY_TO_FIELD_NAME = "_inReplyTo";

	private static final String RESULT_FIELD_NAME = "result";

	/**
	 * Initializes the standard types and fields needed by
	 * all etch generated value factories.
	 * @param types
	 * @param class2type
	 */
	public static final void init( TypeMap types, Class2TypeMap class2type )
	{
		RuntimeExceptionSerializer.init(
			types.get( ETCH_RUNTIME_EXCEPTION_TYPE_NAME ), class2type );
		
		ListSerializer.init(
			types.get( ETCH_LIST_TYPE_NAME ), class2type );
		
		MapSerializer.init(
			types.get( ETCH_MAP_TYPE_NAME ), class2type );
		
		SetSerializer.init(
			types.get( ETCH_SET_TYPE_NAME ), class2type );
		
		DateSerializer.init(
			types.get( ETCH_DATETIME_TYPE_NAME ), class2type );
		
		AuthExceptionSerializer.init(
			types.get( ETCH_AUTH_EXCEPTION_TYPE_NAME ), class2type );

		// ------------------ _mt__Etch_AuthException ------------------
		
		{
			final Type t = types.get( ETCH_EXCEPTION_MESSAGE_NAME );
			t.putValidator( _mf_result, Validator_RuntimeException.get() );
			t.putValidator( _mf__messageId, Validator_long.get( 0 ) );
			t.putValidator( _mf__inReplyTo, Validator_long.get( 0 ) );
		}
	}
	
	/**
	 * The msg field of the standard unchecked exception.
	 */
	public static final Field _mf_msg = new Field( MSG_FIELD_NAME );
	
	/**
	 *  The well-known _messageId field.
	 */
	public static final Field _mf__messageId = new Field( MESSAGE_ID_FIELD_NAME );
	
	/**
	 * The well-known _inReplyTo field.
	 */
	public static final Field _mf__inReplyTo = new Field( IN_REPLY_TO_FIELD_NAME );
	
	/**
	 * The well-known result field.
	 */
	public static final Field _mf_result = new Field( RESULT_FIELD_NAME );

	///////////////////////
	// ValueFactory impl //
	///////////////////////
	
	/**
	 * Constructs the DefaultValueFactory.
	 * @param uri uri of this transport stack.
	 * @param types the type map for this instance of value factory.
	 * @param class2type the class to type map for this instance of value factory.
	 */
	public DefaultValueFactory( String uri, TypeMap types,
		Class2TypeMap class2type )
	{
		URL u = new URL( uri );
		
		level = Level.valueOf( u.getTerm( "DefaultValueFactory.level", "FULL" ) );
		
		this.types = types;
		this.class2type = class2type;
		
		_mt__Etch_RuntimeException = types.get( ETCH_RUNTIME_EXCEPTION_TYPE_NAME );
		_mt__Etch_List = types.get( ETCH_LIST_TYPE_NAME );
		_mt__Etch_Map = types.get( ETCH_MAP_TYPE_NAME );
		_mt__Etch_Set = types.get( ETCH_SET_TYPE_NAME );
		_mt__Etch_Datetime = types.get( ETCH_DATETIME_TYPE_NAME );
		_mt__Etch_AuthException = types.get( ETCH_AUTH_EXCEPTION_TYPE_NAME );
		_mt__exception = types.get( ETCH_EXCEPTION_MESSAGE_NAME );
	}
	
	private final TypeMap types;
	
	private final TypeMap dynamicTypes = new TypeMap();

	private final Class2TypeMap class2type;
	
	private final Type _mt__Etch_RuntimeException;
	
	private final Type _mt__Etch_List;
	
	private final Type _mt__Etch_Map;
	
	private final Type _mt__Etch_Set;
	
	private final Type _mt__Etch_Datetime;
	
	private final Type _mt__Etch_AuthException;
	
	private final Type _mt__exception;
	
	/**
	 * @return the standard _mt__Etch_RuntimeException.
	 */
	public final Type get_mt__Etch_RuntimeException()
	{
		return _mt__Etch_RuntimeException;
	}
	
	/**
	 * @return the standard _mt__Etch_List.
	 */
	public final Type get_mt__Etch_List()
	{
		return _mt__Etch_List;
	}
	
	/**
	 * @return the standard _mt__Etch_Map.
	 */
	public final Type get_mt__Etch_Map()
	{
		return _mt__Etch_Map;
	}
	
	/**
	 * @return the standard _mt__Etch_Set.
	 */
	public final Type get_mt__Etch_Set()
	{
		return _mt__Etch_Set;
	}
	
	/**
	 * @return the standard _mt__Etch_Datetime.
	 */
	public final Type get_mt__Etch_Datetime()
	{
		return _mt__Etch_Datetime;
	}
	
	/**
	 * @return the standard _mt__Etch_AuthException.
	 */
	public final Type get_mt__Etch_AuthException()
	{
		return _mt__Etch_AuthException;
	}
	
	public final Type get_mt__exception()
	{
		return _mt__exception;
	}

	/**
	 * Adds a value factory as a mixin to this one.
	 * @param vf
	 */
	protected void addMixin( ValueFactory vf )
	{
		ValueFactory[] newMixins = new ValueFactory[mixins.length+1];
		System.arraycopy( mixins, 0, newMixins, 0, mixins.length );
		newMixins[mixins.length] = vf;
		mixins = newMixins;
	}
	
	private ValueFactory[] mixins = {};
	
	public final Type getType( Integer id )
	{
		Type type = types.get( id );
		if (type != null)
			return type;
		
		for (ValueFactory vf: mixins)
			if ((type = vf.getType( id )) != null)
				return type;

		synchronized (dynamicTypes)
		{
			return dynamicTypes.get( id );
		}
	}

	public final Type getType( String name )
	{
		Type type = types.get( name );
		if (type != null)
			return type;
		
		for (ValueFactory vf: mixins)
			if ((type = vf.getType( name )) != null)
				return type;
		
		synchronized (dynamicTypes)
		{
			return dynamicTypes.get( name );
		}
	}

	public void addType( Type type )
	{
		try
		{
			synchronized (dynamicTypes)
			{
				dynamicTypes.add( type );
			}
		}
		catch ( IllegalArgumentException e )
		{
			// ignore.
		}
	}
	
	public final Set<Type> getTypes()
	{
		Set<Type> values = types.values();
		
		for (ValueFactory vf: mixins)
			values.addAll( vf.getTypes() );

		synchronized (dynamicTypes)
		{
			values.addAll( dynamicTypes.values() );
		}
		
		return values;
	}

	public final StructValue exportCustomValue( Object value )
	{
		Class<?> clss = value.getClass();
		
		Type type = getCustomStructType( clss );
		
		if (type == null && clss == StructValue.class)
		{
			StructValue struct = (StructValue) value;
			type = struct.type();
		}
		
		if (type == null)
		{
			if (Throwable.class.isAssignableFrom( clss ))
				type = _mt__Etch_RuntimeException;
			else if (List.class.isAssignableFrom( clss ))
				type = _mt__Etch_List;
			else if (Map.class.isAssignableFrom( clss ))
				type = _mt__Etch_Map;
			else if (Set.class.isAssignableFrom( clss ))
				type = _mt__Etch_Set;
			else
				return null;
		}
		
		ImportExportHelper helper = type.getImportExportHelper();
		
		if (helper == null)
			return null;
		
		return helper.exportValue( this, value );
	}

	public final Object importCustomValue( StructValue struct )
	{
		ImportExportHelper helper = struct.type().getImportExportHelper();
		
		if (helper == null)
			return null;
		
		return helper.importValue( struct );
	}

	public final Type getCustomStructType( Class<?> c )
	{
		Type type = class2type.get( c );
		if (type != null)
			return type;
		
		for (ValueFactory vf: mixins)
			if ((type = vf.getCustomStructType( c )) != null)
				return type;
		
		return null;
	}

	/////////////////////
	// STRING ENCODING //
	/////////////////////
	
	public String getStringEncoding()
	{
		return "utf-8";
	}

	////////////////
	// MESSAGE ID //
	////////////////

	public final Long getMessageId( Message msg )
	{
		return (Long) msg.get( _mf__messageId );
	}

	public final void setMessageId( Message msg, Long msgid )
	{
		msg.put( _mf__messageId, msgid );
	}

	public Field get_mf__messageId()
	{
		return _mf__messageId;
	}

	/////////////////
	// IN REPLY TO //
	/////////////////

	public final Long getInReplyTo( Message msg )
	{
		return (Long) msg.get( _mf__inReplyTo );
	}

	public final void setInReplyTo( Message msg, Long msgid )
	{
		msg.put( _mf__inReplyTo, msgid );
	}

	public Field get_mf__inReplyTo()
	{
		return _mf__inReplyTo;
	}

	public void lockDynamicTypes()
	{
		dynamicTypes.lock();
	}

	public Level getLevel()
	{
		return level;
	}

	public Level setLevel( Level level )
	{
		Level oldLevel = this.level;
		this.level = level;
		return oldLevel;
	}

	private Level level = Level.FULL;

	public void unlockDynamicTypes()
	{
		// TODO implement dynamicTypes.unlock();
		throw new UnsupportedOperationException( "unlockDynamicTypes not implemented" );
	}
}
