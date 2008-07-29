/* $Id$
 *
 * Created by sccomer on Feb 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

/**
 * Field is an IdName which denotes a field of a struct
 * or message (i.e., a key for a value).
 */
public final class Field extends IdName
{
	/**
	 * Constructs the Field.
	 * @param id the id of the field.
	 * @param name the name of the field.
	 */
	public Field( int id, String name )
	{
		super( id, name );
	}

	/**
	 * Constructs the Field, computing the appropriate value
	 * for the id.
	 * @param name the name of the field.
	 */
	public Field( String name )
	{
		super( name );
	}
}
