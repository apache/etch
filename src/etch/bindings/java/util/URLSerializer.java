/* $Id$
 *
 * Created by wert on Sep 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.util;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.ImportExportHelper;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.Class2TypeMap;
import etch.bindings.java.support.Validator_string;
import etch.util.URL;

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

	public StructValue exportValue( Object value )
	{
		StructValue struct = new StructValue( type );
		struct.put( field, ((URL) value).toString() );
		return struct;
	}
}
