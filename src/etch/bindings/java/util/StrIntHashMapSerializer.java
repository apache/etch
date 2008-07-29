/* $Id$
 *
 * Created by wert on Jun 25, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.util;

import java.util.Map;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.ImportExportHelper;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.Class2TypeMap;
import etch.bindings.java.support.Validator_object;

/**
 * An etch serializer for StrIntHashMap.
 */
public class StrIntHashMapSerializer implements ImportExportHelper
{
	private final static String FIELD_NAME = "keysAndValues";

	/**
	 * Defines custom fields in the value factory so that the importer
	 * can find them.
	 * @param type
	 * @param class2type 
	 */
	public static void init( Type type, Class2TypeMap class2type )
    {
		Field field = type.getField( FIELD_NAME );
		class2type.put( StrIntHashMap.class, type );
		type.setComponentType( StrIntHashMap.class );
		type.setImportExportHelper( new StrIntHashMapSerializer( type, field ) );
		type.putValidator( field, Validator_object.get( 1 ) );
		type.lock();
    }

	/**
	 * @param type
	 * @param field
	 */
	public StrIntHashMapSerializer( Type type, Field field )
	{
		this.type = type;
		this.field = field;
	}
	
	private final Type type;
	
	private final Field field;
	
	public Object importValue( StructValue struct )
	{
		StrIntHashMap map = new StrIntHashMap();
		
		Object[] keysAndValues = (Object[]) struct.get( field );
		int n = keysAndValues.length;
		int index = 0;
		while (index < n)
		{
			String key = (String) keysAndValues[index++];
			Integer value = (Integer) keysAndValues[index++];
			map.put( key, value );
		}
		
		return map;
	}

	public StructValue exportValue( Object value )
	{
		StrIntHashMap map = (StrIntHashMap) value;
		
		Object[] keysAndValues = new Object[map.size()*2];
		int index = 0;
		for (Map.Entry<String, Integer> me: map.entrySet())
		{
			keysAndValues[index++] = me.getKey();
			keysAndValues[index++] = me.getValue();
		}
		
		StructValue struct = new StructValue( type );
		struct.put( field, keysAndValues );
		return struct;
	}
}
