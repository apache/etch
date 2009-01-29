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

package org.apache.etch.util.cmd;

import org.apache.etch.util.Assertion;

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
