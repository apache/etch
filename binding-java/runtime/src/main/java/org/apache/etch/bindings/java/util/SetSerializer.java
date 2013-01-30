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

import java.util.HashSet;
import java.util.Set;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.ImportExportHelper;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.Validator_object;


/**
 * Serializer for generic Set<Object>.
 */
public class SetSerializer implements ImportExportHelper
{
	private final static String FIELD_NAME = "keys";

	/**
	 * Defines custom fields in the value factory so that the importer
	 * can find them.
	 * @param type
	 * @param class2type
	 */
	public static void init( Type type, Class2TypeMap class2type )
    {
		Field field = type.getField( FIELD_NAME );
		
		class2type.put( HashSet.class, type );
		type.setComponentType( Set.class );
		
		type.setImportExportHelper( new SetSerializer( type, field ) );
		type.putValidator( field, Validator_object.get( 1 ) );
		type.lock();
    }

	/**
	 * @param type
	 * @param field
	 */
	public SetSerializer( Type type, Field field )
	{
		this.type = type;
		this.field = field;
	}
	
	private final Type type;
	
	private final Field field;
	
	public Object importValue( StructValue struct )
	{
		struct.checkType( type );
		Object[] keys = (Object[]) struct.get( field );
		
		Set<Object> set = new HashSet<Object>( (keys.length*4+2)/3, .75f );
		for (Object key: keys)
			set.add( key );
		
		return set;
	}

	@SuppressWarnings("unchecked")
	public StructValue exportValue( ValueFactory vf, Object value )
	{
		Set<Object> set = (Set<Object>) value;
		
		Object[] keys = new Object[set.size()];
		set.toArray( keys );
		
		StructValue struct = new StructValue( type, vf );
		struct.put( field, keys );
		return struct;
	}
}
