/* $Id: IntegerOption.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Mar 27, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util.cmd;

/**
 * An option whose value is an integer.
 */
public class IntegerOption extends Option
{
	/**
	 * Method should be declared as:
	 * 
	 * public boolean method( CommandParser cp, Option option, String token, Integer value );
	 * 
	 * @param cp
	 * @param tokens
	 * @param name
	 * @param method
	 * @param description
	 * @param flags
	 * @param defaultValue
	 * @param constraint
	 * @throws Exception
	 */
	public IntegerOption( CommandParser cp, String tokens, String name, String method,
		String description, int flags, Integer defaultValue, Constraint constraint )
		throws Exception
	{
		super( cp, tokens, name, method, PARAMS, description, flags, defaultValue, constraint );
	}
	
	private final static Class<?>[] PARAMS =
		{ CommandParser.class, Option.class, String.class, Integer.class };

	@Override
	protected Object convertValue( String value )
	{
		return new Integer( value );
	}
}
