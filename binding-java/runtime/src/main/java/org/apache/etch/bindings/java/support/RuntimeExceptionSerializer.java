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

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.ImportExportHelper;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;

/**
 * Serializer for RuntimeException.
 */
public class RuntimeExceptionSerializer implements ImportExportHelper
{
	private final static String FIELD_NAME = "msg";

	/**
	 * Defines custom fields in the value factory so that the importer
	 * can find them.
	 * @param type
	 * @param class2type
	 */
	public static void init( Type type, Class2TypeMap class2type )
	{
		Field field = type.getField( FIELD_NAME );
		class2type.put( _Etch_RuntimeException.class, type );
		type.setComponentType( _Etch_RuntimeException.class );
		type.setImportExportHelper( new RuntimeExceptionSerializer( type, field ) );
		type.putValidator( field, Validator_string.get( 0 ) );
		type.lock();
	}

	/**
	 * Constructs the RuntimeExceptionSerializer.
	 *
	 * @param type
	 * @param field
	 */
	public RuntimeExceptionSerializer( Type type, Field field )
	{
		this.type = type;
		this.field = field;
	}
	
	private final Type type;
	
	private final Field field;

	public StructValue exportValue( ValueFactory vf, Object value )
	{
		StructValue sv = new StructValue( type, vf );
		sv.put( field, value.toString() );
		return sv;
	}

	public Object importValue( StructValue struct )
	{
		struct.checkType( type );
		return new _Etch_RuntimeException( (String) struct.get( field ) );
	}
}