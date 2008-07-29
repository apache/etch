/* $Id$
 *
 * Created by sccomer on Mar 29, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

/**
 * Interface to implementation to help compare conformant but not
 * identical objects found in struct value and array value instances.
 */
public interface EqualsHelper
{
	/**
	 * @param a
	 * @param b
	 * @return true if the value of a and b are equal even though
	 * the format is not (e.g., 23 as a Byte vs. Integer).
	 */
	public boolean equals( Object a, Object b );
}
