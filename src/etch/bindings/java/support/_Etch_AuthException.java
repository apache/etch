/* $Id$
 *
 * Created by wert on Jul 24, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

/**
 * Exception thrown when messages marked with @Authorize fail.
 */
public class _Etch_AuthException extends _Etch_RuntimeException
{
	private static final long serialVersionUID = -5541931265968145750L;

	/**
	 * Constructs the _Etch_AuthException.
	 */
	public _Etch_AuthException()
	{
		// don't init anything.
	}

	/**
	 * Constructs the _Etch_AuthException.
	 * 
	 * @param msg a message describing the failure.
	 */
	public _Etch_AuthException( String msg )
	{
		super( msg );
	}
}
