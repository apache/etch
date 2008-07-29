/* $Id: NullOption.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Mar 27, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util.cmd;

import etch.util.Assertion;

/**
 * An option which has no value.
 */
public class NullOption extends Option
{
	/**
	 * @param cp
	 * @param tokens
	 * @param method
	 * @param description
	 * @param flags
	 * @throws Exception
	 */
	public NullOption( CommandParser cp, String tokens, String method,
		String description, int flags ) throws Exception
	{
		super( cp, tokens, (String[]) null, method, PARAMS, description, flags, null, null );
	}
	
	private final static Class<?>[] PARAMS =
		{ CommandParser.class, Option.class, String.class };
	
	@Override
	public boolean wantsValue()
	{
		return false;
	}

	@Override
	protected Object convertValue( String value )
	{
		throw new UnsupportedOperationException();
	}

	@Override
	protected Object[] getArgs( String token, Object value )
	{
		Assertion.check( value == null, "value == null" );
		return new Object[] { getCommandParser(), this, token };
	}
}
