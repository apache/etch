/* $Id: Assertion.java 23004 2006-05-24 22:25:57Z wert $
 *
 * Created by wert on Jan 25, 2005
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.util;

/**
 * Portable assertion checking.
 */
public class Assertion
{
	/**
	 * Checks a value for being true and if not it throws an exception
	 * with the specified description in the message.
	 * 
	 * @param value a boolean value which should be true (e.g., x < 100 ).
	 * 
	 * @param msg a description of value (e.g., "x < 100" ).
	 * 
	 * @throws AssertionException when value is not true.
	 */
	public static void check( boolean value, String msg )
		throws AssertionException
	{
		if (!value)
			throw new AssertionException( "assertion failed: "+msg );
	}

	/**
	 * Checks a value for being true and if not it throws an exception
	 * with the specified description in the message.
	 * 
	 * @param value a boolean value which should be true (e.g., x < 100 ).
	 * 
	 * @param fmt a description of value (e.g., "x (%d) < 100" ).
	 * 
	 * @param args args for the format.
	 * 
	 * @throws AssertionException when value is not true.
	 */
	public static void check( boolean value, String fmt, Object ... args )
		throws AssertionException
	{
		if (!value)
			throw new AssertionException( "assertion failed: "+
				String.format( fmt, args ) );
	}
}
