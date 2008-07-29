/* $Id$
 *
 * Created by sccomer on Apr 18, 2008.
 *
 * Copyright (c) 2005 Metreos, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.ImportExportHelper;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;

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

	public StructValue exportValue( Object value )
	{
		StructValue sv = new StructValue( type );
		sv.put( field, value.toString() );
		return sv;
	}

	public Object importValue( StructValue struct )
	{
		struct.checkType( type );
		return new _Etch_RuntimeException( (String) struct.get( field ) );
	}
}