/* $Id$
 *
 * Created by wert on Jun 25, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.util;

import java.util.HashMap;

/**
 * A HashMap which is from String to String.
 * 
 * To use StrStrHashMap in an etch service, define an extern type and prefix
 * it with this annotation:
 * 
 * @Extern( java, "etch.bindings.java.util.StrStrHashMap", "",
 *   "etch.bindings.java.util.StrStrHashMapSerializer", "" )
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
