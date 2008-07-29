/* $Id$
 *
 * Created by Champakesan, Badri Narayanan on Jul 23, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.util;

import java.util.Iterator;
import java.util.Map;

import etch.bindings.java.msg.ArrayValue;
import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator_ArrayValue;
import etch.bindings.java.msg.ValueFactory;

/**
 * An etch serializer for StrStrHashMap.
 */
public class StrStrHashMapSerializer
{
	private final static String FIELD_NAME = "keysAndValues";
	
	/**
	 * @param vf
	 * @param struct of imported value.
	 * @return imported value.
	 */
	public static StrStrHashMap importValue( ValueFactory vf, StructValue struct )
	{
		StrStrHashMap map = new StrStrHashMap();
		ArrayValue av = (ArrayValue) struct.get( vf.getField( FIELD_NAME ) );
		Iterator<Object> i = av.iterator();
		while (i.hasNext())
		{
			String key = (String) i.next();
			String value = (String) i.next();
			map.put( key, value );
		}
		return map;
	}

	/**
	 * @param vf
	 * @param value to be exported.
	 * @param struct to export value to.
	 */
	public static void exportValue( ValueFactory vf, StrStrHashMap value,
		StructValue struct )
	{
		ArrayValue av = new ArrayValue();
		struct.put( vf.getField( FIELD_NAME ), av );
		for (Map.Entry<String, String> me: value.entrySet())
		{
			av.add( me.getKey() );
			av.add( me.getValue() );
		}
	}

	/**
	 * Defines custom fields in the value factory so that the importer
	 * can find them.
	 * @param vf
	 * @param type 
	 */
	public static void init( ValueFactory vf, Type type )
    {
		Field field = vf.getField( FIELD_NAME );
		type.putValidator( field, Validator_ArrayValue.get() );
    }
}
