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

package org.apache.etch.bindings.java.util;

import java.util.Map;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.ImportExportHelper;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.Validator_object;


/**
 * An etch serializer for StrStrHashMap.
 */
public class StrStrHashMapSerializer implements ImportExportHelper
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
		class2type.put( StrStrHashMap.class, type );
		type.setComponentType( StrStrHashMap.class );
		type.setImportExportHelper( new StrStrHashMapSerializer( type, field ) );
		type.putValidator( field, Validator_object.get( 1 ) );
		type.lock();
    }

	/**
	 * @param type
	 * @param field
	 */
	public StrStrHashMapSerializer( Type type, Field field )
	{
		this.type = type;
		this.field = field;
	}
	
	private final Type type;
	
	private final Field field;
	
	public Object importValue( StructValue struct )
	{
		StrStrHashMap map = new StrStrHashMap();
		
		Object[] keysAndValues = (Object[]) struct.get( field );
		int n = keysAndValues.length;
		int index = 0;
		while (index < n)
		{
			String key = (String) keysAndValues[index++];
			String value = (String) keysAndValues[index++];
			map.put( key, value );
		}
		
		return map;
	}

	public StructValue exportValue( ValueFactory vf, Object value )
	{
		StrStrHashMap map = (StrStrHashMap) value;
		
		Object[] keysAndValues = new Object[map.size()*2];
		int index = 0;
		for (Map.Entry<String, String> me: map.entrySet())
		{
			keysAndValues[index++] = me.getKey();
			keysAndValues[index++] = me.getValue();
		}
		
		StructValue struct = new StructValue( type, vf );
		struct.put( field, keysAndValues );
		return struct;
	}
}
