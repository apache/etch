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

package org.apache.etch.bindings.java.util;

import java.util.HashMap;

/**
 * A HashMap which is from String to String.
 * 
 * To use StrStrHashMap in an etch service, define an extern type and prefix
 * it with this annotation:
 * 
 * @Extern( java, "org.apache.etch.bindings.java.util.StrStrHashMap", "",
 *   "org.apache.etch.bindings.java.util.StrStrHashMapSerializer", "" )
 * extern StrStrHashMap
 */
public class StrStrHashMap extends HashMap<String, String>
{
	private static final long serialVersionUID = 6910845292628589771L;

	/**
	 * Constructs a StrStrHashMap initialized from another.
	 * @param other
	 */
	public StrStrHashMap( StrStrHashMap other )
	{
		super( other );
	}
	
	/**
	 * Constructs a StrStrHashMap.
	 */
	public StrStrHashMap()
	{
		// nothing to do.
	}
}
