/* $Id$
 *
 * Copyright 2009-2010 Cisco Systems Inc.
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
package cisco.uc.cuae.etchrouter.utils;

import java.lang.reflect.Constructor;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;

public class LocalStructImportExportHelper extends LocalTypeImportExportHelper
{

	private List<String> _fieldNames = null;
	
	private Map<String, org.apache.etch.bindings.java.msg.Field> _fieldMap = null;
	
	private Map<String, java.lang.reflect.Field> _reflectFieldMap = null;
	
	private Constructor _constructor = null;

	public LocalStructImportExportHelper( Type structType, List<String> fieldNames)
		throws Exception
	{
		super( structType );
		_fieldNames = fieldNames;
		initAndValidate();
	}
	
	private void initAndValidate() throws Exception
	{
		String typeName = _type.getName();
		if (_typeClass.isEnum())
			throw new Exception(String.format( "The loaded class type '%s' is not struct or exception type", typeName ));
		_constructor = _typeClass.getConstructor( );
		java.lang.reflect.Field[] publicFields = _typeClass.getFields();
		if (publicFields.length!=_fieldNames.size())
			throw new Exception(String.format( 
				"The number of public fields in class type '%s' is %s, but the size of field name list is %s",
				typeName, publicFields.length, _fieldNames.size() ));
		_fieldMap = new HashMap<String, org.apache.etch.bindings.java.msg.Field>(publicFields.length);
		_reflectFieldMap = new HashMap<String, java.lang.reflect.Field>(publicFields.length);
		for (java.lang.reflect.Field reflectField : publicFields)
		{
			String name = reflectField.getName();
			if (!_fieldNames.contains( name ))
				throw new Exception(String.format( "The public field '%s' in class type '%s' is not included in the field name list", name, typeName ));
			_fieldMap.put( name, new org.apache.etch.bindings.java.msg.Field(name) );
			_reflectFieldMap.put( name, reflectField );
		}
	}
	
	public StructValue exportValue( ValueFactory vf, Object value )
	{
		StructValue struct = new StructValue( _type, vf );
		try
		{
			for (String name : _fieldNames)
			{
				struct.put( _fieldMap.get( name ), _reflectFieldMap.get( name ).get( value ) );
			}
		}
		catch (Exception e)
		{
			throw new RuntimeException(e);
		}
		return struct;
	}

	public Object importValue( StructValue struct )
	{
		try
		{
			Object instance = _constructor.newInstance( );
			for (String name : _fieldNames)
			{
				_reflectFieldMap.get( name ).set( instance, struct.get( _fieldMap.get( name ) ) );
			}
			return instance;
		}
		catch (Exception e)
		{
			throw new RuntimeException(e);
		}
	}

}
