/* $Id$
 *
 * Created by sccomer on Apr 14, 2008.
 *
 * Copyright (c) 2005 Metreos, Inc. All rights reserved.
 */

package etch.bindings.java.util;

import java.util.ArrayList;
import java.util.List;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.ImportExportHelper;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.Class2TypeMap;
import etch.bindings.java.support.Validator_object;

/**
 * Serializer for generic List<Object>.
 */
public class ListSerializer implements ImportExportHelper
{
	private final static String FIELD_NAME = "values";

	/**
	 * Defines custom fields in the value factory so that the importer
	 * can find them.
	 * @param type
	 * @param class2type
	 */
	public static void init( Type type, Class2TypeMap class2type )
    {
		Field field = type.getField( FIELD_NAME );
		class2type.put( ArrayList.class, type );
		type.setComponentType( List.class );
		type.setImportExportHelper( new ListSerializer( type, field ) );
		type.putValidator( field, Validator_object.get( 1 ) );
		type.lock();
    }

	/**
	 * @param type
	 * @param field
	 */
	public ListSerializer( Type type, Field field )
	{
		this.type = type;
		this.field = field;
	}
	
	private final Type type;
	
	private final Field field;
	
	public Object importValue( StructValue struct )
	{
		struct.checkType( type );
		Object[] values = (Object[]) struct.get( field );
		
		List<Object> list = new ArrayList<Object>( values.length );
		for( Object value: values)
			list.add( value );
		
		return list;
	}

	@SuppressWarnings("unchecked")
	public StructValue exportValue( Object value )
	{
		List<Object> list = (List<Object>) value;
		
		Object[] values = new Object[list.size()];
		list.toArray( values );
		
		StructValue struct = new StructValue( type );
		struct.put( field, values );
		return struct;
	}
}
