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

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.ImportExportHelper;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.Validator_string;
import org.apache.etch.util.URL;


/**
 * An etch serializer for URL.
 */
public class URLSerializer implements ImportExportHelper
{
	private final static String FIELD_NAME = "urlStr";

	/**
	 * Defines custom fields in the value factory so that the importer
	 * can find them.
	 * @param type
	 * @param class2type
	 */
	public static void init( Type type, Class2TypeMap class2type )
	{
		Field field = type.getField( FIELD_NAME );
		class2type.put( URL.class, type );
		type.setComponentType( URL.class );
		type.setImportExportHelper( new URLSerializer( type, field ) );
		type.putValidator( field, Validator_string.get( 0 ) );
		type.lock();
	}

	/**
	 * @param type
	 * @param field
	 */
	public URLSerializer( Type type, Field field )
	{
		this.type = type;
		this.field = field;
	}
	
	private final Type type;
	
	private final Field field;

	public Object importValue( StructValue struct )
	{
		return new URL( (String) struct.get( field ) );
	}

	public StructValue exportValue( ValueFactory vf, Object value )
	{
		StructValue struct = new StructValue( type, vf );
		struct.put( field, ((URL) value).toString() );
		return struct;
	}
}
