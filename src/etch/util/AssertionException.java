/* $Id: AssertionException.java 22115 2006-04-21 22:00:58Z wert $
 *
 * Created by wert on Jan 25, 2005
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util;

/**
 * Exception thrown upon assertion failure.
 * 
 * @see Assertion#check(boolean, String, Object[])
 */
public class AssertionException extends RuntimeException
{
	/**
	 * 
	 */
	private static final long serialVersionUID = 3545520603445735478L;

	/**
	 * Constructs an assertion exception.
	 * 
	 * @param message describes the failure.
	 */
	public AssertionException( String message )
	{
		super( message );
	}
}
