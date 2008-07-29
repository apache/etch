/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
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
