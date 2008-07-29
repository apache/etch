/* $Id$
 *
 * Created by wert on Jun 25, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.util;

import java.util.HashMap;

/**
 * A HashMap which is from String to Integer.
 */
public class StrIntHashMap extends HashMap<String, Integer>
{
	private static final long serialVersionUID = -9162231227512652630L;

	/**
	 * Constructs a StrIntHashMap initialized from another.
	 * @param other
	 */
	public StrIntHashMap( StrIntHashMap other )
	{
		super( other );
	}
	
	/**
	 * Constructs a StrIntHashMap.
	 */
	public StrIntHashMap()
	{
		// nothing to do.
	}
}
