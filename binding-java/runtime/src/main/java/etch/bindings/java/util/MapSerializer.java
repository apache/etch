/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.bindings.java.util;

import java.util.HashMap;
import java.util.Map;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.ImportExportHelper;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.Class2TypeMap;
import etch.bindings.java.support.Validator_object;

/**
 * Serializer for generic Map<Object, Object>.
 */
public class MapSerializer implements ImportExportHelper
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
		
		// quick lookup to serialize this class.
		class2type.put( HashMap.class, type );
		
		// type for arrays.
		type.setComponentType( Map.class );
		
		type.setImportExportHelper( new MapSerializer( type, field ) );
		type.putValidator( field, Validator_object.get( 1 ) );
		type.lock();
    }

	/**
	 * @param type
	 * @param field
	 */
	public MapSerializer( Type type, Field field )
	{
		this.type = type;
		this.field = field;
	}
	
	private final Type type;
	
	private final Field field;
	
	public Object importValue( StructValue struct )
	{
		struct.checkType( type );
		Object[] keysAndValues = (Object[]) struct.get( field );
		int n = keysAndValues.length;
		
		Map<Object, Object> map = new HashMap<Object, Object>( (n*2+2)/3, 0.75f ); // (n/2)*4/3
		int index = 0;
		while (index < n)
		{
			Object key = keysAndValues[index++];
			Object value = keysAndValues[index++];
			map.put( key, value );
		}
		
		return map;
	}

	@SuppressWarnings("unchecked")
	public StructValue exportValue( ValueFactory vf, Object value )
	{
		Map<Object, Object> map = (Map<Object, Object>) value;
		
		Object[] keysAndValues = new Object[map.size()*2];
		int index = 0;
		for (java.util.Map.Entry<Object, Object> me: map.entrySet())
		{
			keysAndValues[index++] = me.getKey();
			keysAndValues[index++] = me.getValue();
		}
		
		StructValue struct = new StructValue( type, vf );
		struct.put( field, keysAndValues );
		return struct;
	}
}
