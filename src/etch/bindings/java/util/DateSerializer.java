/* $Id$
 *
 * Created by wert on Sep 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.util;

import java.util.Date;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.ImportExportHelper;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.Class2TypeMap;
import etch.bindings.java.support.Validator_long;

/**
 * An etch serializer for Date.
 */
public class DateSerializer implements ImportExportHelper
{
	private final static String FIELD_NAME = "dateTime";

	/**
	 * Defines custom fields in the value factory so that the importer
	 * can find them.
	 * @param type
	 * @param class2type
	 */
	public static void init( Type type, Class2TypeMap class2type )
	{
		Field field = type.getField( FIELD_NAME );
		class2type.put( Date.class, type );
		type.setComponentType( Date.class );
		type.setImportExportHelper( new DateSerializer( type, field ) );
		type.putValidator( field, Validator_long.get( 0 ) );
		type.lock();
	}

	/**
	 * @param type
	 * @param field
	 */
	public DateSerializer( Type type, Field field )
	{
		this.type = type;
		this.field = field;
	}
	
	private final Type type;
	
	private final Field field;

	public Object importValue( StructValue struct )
	{
		return new Date( (Long) struct.get( field ) );
	}

	public StructValue exportValue( Object value )
	{
		StructValue struct = new StructValue( type );
		struct.put( field, ((Date) value).getTime() );
		return struct;
	}
}
