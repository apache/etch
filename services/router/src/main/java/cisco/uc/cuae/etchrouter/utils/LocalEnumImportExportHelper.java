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

import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.ValueFactory;

public class LocalEnumImportExportHelper extends LocalTypeImportExportHelper
{	
	private List<String> _entryValues = null;
	
	private Map<String, Object> _enumConstMap = null;
	
	private Method _nameMethod = null;
	
	private Map<String, etch.bindings.java.msg.Field> _fieldMap = null;
	
	public LocalEnumImportExportHelper( Type enumType, List<String> entryValues )
		throws Exception
	{
		super( enumType );
		_entryValues = entryValues;
		initAndValidate();
	}
	
	private void initAndValidate() throws Exception
	{
		String typeName = _type.getName();
		if (!_typeClass.isEnum())
			throw new Exception(String.format( "The loaded class type '%s' is not an enum type", typeName ));
		Object[] enumValues = _typeClass.getEnumConstants();
		if (_entryValues.size()!=enumValues.length)
			throw new Exception(String.format( 
				"The number of declared enum fields in Enum type '%s' is %s, but the size of entry value list is %s",
				typeName, enumValues.length, _entryValues.size() ));
		_enumConstMap = new HashMap<String, Object>(_entryValues.size());
		_fieldMap = new HashMap<String, etch.bindings.java.msg.Field>();
		_nameMethod = _typeClass.getMethod( "name", null );
		for (int i=0; i<enumValues.length; i++)
		{
			String name = (String)(_nameMethod.invoke( enumValues[i], null ));
			if (!_entryValues.contains( name ))
				throw new Exception(String.format( "The declared field '%s' in enum type '%s' is not included in the entry value list", name, typeName ));
			_enumConstMap.put( name, enumValues[i] );
			_fieldMap.put( name, new etch.bindings.java.msg.Field(name) );
		}
	}
	
	public StructValue exportValue( ValueFactory vf, Object value )
	{
		StructValue struct = new StructValue( _type, vf );
		try
		{
			String name = (String)_nameMethod.invoke( value, null );
			struct.put( _fieldMap.get( name ), true );
		}
		catch (Exception e)
		{
			throw new RuntimeException(e);
		}

		return struct;
	}

	public Object importValue( StructValue struct )
	{
		etch.bindings.java.msg.Field key = struct.keySet().iterator().next();
		return _enumConstMap.get( key.getName() );
	}

}
