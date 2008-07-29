/* $Id: UrlOption.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Mar 27, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util.cmd;

import etch.util.URL;

/**
 * An option whose value is a url.
 */
public class UrlOption extends Option
{
	/**
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
	public UrlOption( CommandParser cp, String tokens, String name,
		String method, String description, int flags, URL defaultValue,
		Constraint constraint ) throws Exception
	{
		super( cp, tokens, name, method, PARAMS, description, flags,
			defaultValue, constraint );
	}
	
	private final static Class<?>[] PARAMS =
		{ CommandParser.class, Option.class, String.class, URL.class };

	@Override
	protected Object convertValue( String value )
	{
		return new URL( value );
	}
}
