/* $Id$
 *
 * Created by sccomer on Apr 14, 2008.
 *
 * Copyright (c) 2005 Metreos, Inc. All rights reserved.
 */

package etch.bindings.java.util;

import java.util.HashSet;
import java.util.Set;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.ImportExportHelper;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.Class2TypeMap;
import etch.bindings.java.support.Validator_object;

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
	public StructValue exportValue( Object value )
	{
		Set<Object> set = (Set<Object>) value;
		
		Object[] keys = new Object[set.size()];
		set.toArray( keys );
		
		StructValue struct = new StructValue( type );
		struct.put( field, keys );
		return struct;
	}
}
