/* $Id$
 *
 * Created by sccomer on Mar 29, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import java.util.Collection;
import java.util.Date;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator_RuntimeException;
import etch.bindings.java.msg.Validator_long;
import etch.bindings.java.msg.Validator_string;
import etch.bindings.java.msg.ValueFactory;

/**
 * Default implementation of ValueFactory which provides some
 * dynamic type and field support, as well as standard
 * value conversions and import and rt.
 */
public class DefaultValueFactory implements ValueFactory
{
	/**
	 * Constructs the DefaultValueFactory with up to 100 automatic
	 * type and field declarations.
	 */
	public DefaultValueFactory()
	{
		this( 100 );
	}
	
	/**
	 * Constructs the DefaultValueFactory.
	 * @param maxAutoCount the maximum number of automatic type
	 * and field declarations (counted separately).
	 */
	public DefaultValueFactory( int maxAutoCount )
	{
		types = new IdNameMap<Type>( maxAutoCount )
		{
			@Override
			public Type makeNew( Integer id, String name )
			{
				return new Type( id, name );
			}
		};
		
		fields = new IdNameMap<Field>( maxAutoCount )
		{
			@Override
			public Field makeNew( Integer id, String name )
			{
				return new Field( id, name );
			}
		};
		
		addType( _mt__Etch_RuntimeException );
		addType( _mt__Etch_AuthException );
		addType( _mt__Etch_Date );
		addType( _mt__exception );
		
		addField( _mf_msg );
		addField( _mf_ticks );
		addField( _mf__messageId );
		addField( _mf__inReplyTo );
		addField( _mf_result );
	}
	
	/**
	 * The type of the standard unchecked exception.
	 */
	public static final Type _mt__Etch_RuntimeException = new Type( -284614264, "_Etch_RuntimeException" );
	
	/**
	 * The type of the standard auth exception.
	 */
	public static final Type _mt__Etch_AuthException = new Type( -1746727050, "_Etch_AuthException" );
	
	/**
	 * The type of the standard system date.
	 */
	public static final Type _mt__Etch_Date = new Type( -1201742561, "_Etch_Date" );
	
	/**
	 * The type of the response to a oneway message which throws an exception.
	 */
	public static final Type _mt__exception = new Type( -1522717131, "_exception" );
	
	/**
	 * The msg field of the standard unchecked exception.
	 */
	public static final Field _mf_msg = new Field( 769750364, "msg" );
	
	/**
	 * The ticks field of the standard system date.
	 */
	public static final Field _mf_ticks = new Field( 543806897, "ticks" );
	
	/**
	 *  The well-known _messageId field.
	 */
	public static final Field _mf__messageId = new Field( 1661383784, "_messageId" );
	
	/**
	 * The well-known _inReplyTo field.
	 */
	public static final Field _mf__inReplyTo = new Field( -307705434, "_inReplyTo" );
	
	/**
	 * The well-known result field.
	 */
	public static final Field _mf_result = new Field( -2130379326, "result" );

	////////////////
	// VALIDATORS //
	////////////////
	
	static
	{
		_mt__Etch_RuntimeException.putValidator( _mf_msg, Validator_string.get( 0 ) );
		
		_mt__Etch_AuthException.putValidator( _mf_msg, Validator_string.get( 0 ) );
		
		_mt__Etch_Date.putValidator( _mf_ticks, Validator_long.get( 0 ) );
		
		_mt__exception.putValidator( _mf_result, Validator_RuntimeException.get() );
		_mt__exception.putValidator( _mf__messageId, Validator_long.get( 0 ) );
		_mt__exception.putValidator( _mf__inReplyTo, Validator_long.get( 0 ) );
	}
	
	//////////
	// TYPE //
	//////////
	
	public Type addType( Type type )
	{
		return types.add( type );
	}
	
	public Type getType( Integer id )
	{
		// check the well-known types
		
		switch (id)
		{
			case -284614264: return _mt__Etch_RuntimeException;
			case -1746727050: return _mt__Etch_AuthException;
			case -1201742561: return _mt__Etch_Date;
			case -1522717131: return _mt__exception;
		}
		
		// check the dynamic types
		
		return types.get( id );
	}

	public Type getType( String name )
	{
		return types.get( name );
	}
	
	public Collection<Type> getTypes()
	{
		return types.values();
	}
	
	private final IdNameMap<Type> types;

	///////////
	// FIELD //
	///////////
	
	public Field addField( Field mf )
	{
		return fields.add( mf );
	}

	public Field getField( Integer id )
	{
		// check the well-known fields
		
		switch (id)
		{
			case 769750364: return _mf_msg;
			case 543806897: return _mf_ticks;
			case 1661383784: return _mf__messageId;
			case -307705434: return _mf__inReplyTo;
			case -2130379326: return _mf_result;
		}
		
		// check the dynamic fields
		
		return fields.get( id );
	}

	public Field getField( String name )
	{
		return fields.get( name );
	}
	
	public Collection<Field> getFields()
	{
		return fields.values();
	}
	
	private final IdNameMap<Field> fields;

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

	public Long getMessageId( Message msg )
	{
		return (Long) msg.get( _mf__messageId );
	}

	public void setMessageId( Message msg, Long msgid )
	{
		if (msgid != null)
			msg.put( _mf__messageId, msgid );
		else
			msg.remove( _mf__messageId );
	}

	/////////////////
	// IN REPLY TO //
	/////////////////

	public Long getInReplyTo( Message msg )
	{
		return (Long) msg.get( _mf__inReplyTo );
	}

	public void setInReplyTo( Message msg, Long msgid )
	{
		if (msgid != null)
			msg.put( _mf__inReplyTo, msgid );
		else
			msg.remove( _mf__inReplyTo );
	}

	//////////////////////
	// VALUE CONVERSION //
	//////////////////////

	public StructValue exportCustomValue( Object value )
	{
		Class<?> c = value.getClass();

		if (c == Date.class)
		{
			Date v = (Date) value;
			StructValue sv = new StructValue( _mt__Etch_Date );
			sv.put( _mf_ticks, v.getTime() );
			return sv;
		}
		
		if (c == _Etch_AuthException.class)
		{
			_Etch_AuthException v = (_Etch_AuthException) value;
			StructValue sv = new StructValue( _mt__Etch_AuthException );
			sv.put( _mf_msg, v.toString() );
			return sv;
		}
		
		// catch any exception which wasn't otherwise
		// handled and pass it through.

		if (value instanceof Exception)
		{
			Exception v = (Exception) value;
			StructValue sv = new StructValue( _mt__Etch_RuntimeException );
			sv.put( _mf_msg, v.toString() );
			return sv;
		}
		
		return null;
	}

	public Object importCustomValue( StructValue sv )
	{
		Type type = sv.type();
		
		switch (type.getId())
		{
			case -284614264: // _mt__Etch_RuntimeException
				return new _Etch_RuntimeException( (String) sv.get( _mf_msg ) );
			
			case -1746727050: // _mt__Etch_AuthException
				return new _Etch_AuthException( (String) sv.get( _mf_msg ) );
			
			case -1201742561: // _mt__Etch_Date
				return new Date( (Long) sv.get( _mf_ticks ) );
		}
		
		return null;
	}

	public Type getCustomStructType( Class<?> c )
	{
		if (c == Date.class)
			return _mt__Etch_Date;
		
		return null;
	}

	public Class<?> getCustomType( Type type )
	{
		switch (type.getId())
		{
			case -1201742561: // _mt__Etch_Date
				return Date.class;
		}
		
		return null;
	}
	
//	/**
//	 * Converts a n-dimensional (including 0) ArrayValue into a similarly
//	 * shaped array of clss.
//	 * @param obj level-n nested ArrayValue
//	 * @param clss the target class (e.g., int[].class for int[]).
//	 * @return converted value.
//	 */
//	public final static Object arrayValue2Native( Object obj, Class<?> clss )
//	{
//		if (obj == null)
//			return null;
//		
//		if (!clss.isArray())
//			return obj;
//		
//		if (!(obj instanceof ArrayValue))
//			return obj;
//		
//		ArrayValue av = (ArrayValue) obj;
//		int k = av.size();
//		
//		clss = clss.getComponentType();
//		int[] dims = { k };
//		Object v = Array.newInstance( clss, dims );
//		
//		for (int i = 0; i < k; i++)
//			Array.set( v, i, arrayValue2Native( av.get( i ), clss ) );
//		
//		return v;
//	}

//	/**
//	 * @param c the target class.
//	 * @param obj ArrayValue conformant to n-dimensional boolean array.
//	 * @return n-dimensional boolean array.
//	 */
//	public final static Object to_boolean( Class<?> c, Object obj )
//	{
//		return arrayValue2Native( obj, c );
//	}

//	/**
//	 * @param c the target class.
//	 * @param obj ArrayValue conformant to n-dimensional byte array.
//	 * @return n-dimensional byte array.
//	 */
//	public final static Object to_byte( Class<?> c, Object obj )
//	{
//		return arrayValue2Native( obj, c );
//	}

//	/**
//	 * @param c the target class.
//	 * @param obj ArrayValue conformant to n-dimensional short array.
//	 * @return n-dimensional short array.
//	 */
//	public final static Object to_short( Class<?> c, Object obj )
//	{
//		return arrayValue2Native( obj, c );
//	}

//	/**
//	 * @param c the target class.
//	 * @param obj ArrayValue conformant to n-dimensional int array.
//	 * @return n-dimensional int array.
//	 */
//	public final static Object to_int( Class<?> c, Object obj )
//	{
//		return arrayValue2Native( obj, c );
//	}

//	/**
//	 * @param c the target class.
//	 * @param obj ArrayValue conformant to n-dimensional long array.
//	 * @return n-dimensional long array.
//	 */
//	public final static Object to_long( Class<?> c, Object obj )
//	{
//		return arrayValue2Native( obj, c );
//	}

//	/**
//	 * @param c the target class.
//	 * @param obj ArrayValue conformant to n-dimensional float array.
//	 * @return n-dimensional float array.
//	 */
//	public final static Object to_float( Class<?> c, Object obj )
//	{
//		return arrayValue2Native( obj, c );
//	}

//	/**
//	 * @param c the target class.
//	 * @param obj ArrayValue conformant to n-dimensional double array.
//	 * @return n-dimensional double array.
//	 */
//	public final static Object to_double( Class<?> c, Object obj )
//	{
//		return arrayValue2Native( obj, c );
//	}

//	/**
//	 * @param c the target class.
//	 * @param obj ArrayValue conformant to n-dimensional String array.
//	 * @return n-dimensional String array.
//	 */
//	public final static Object to_string( Class<?> c, Object obj )
//	{
//		return arrayValue2Native( obj, c );
//	}
}
