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

package etch.bindings.java.support;

import etch.util.core.io.Session;

/**
 * Default implementation of ServerFactory. Used by Etch generated Helper files
 * to provide listeners with backstop implementations of Session methods which
 * forward to user's implementation factory.
 */
abstract public class DefaultServerFactory implements ServerFactory
{
	/**
	 * Constructs the DefaultServerFactory.
	 * @param implFactory
	 */
	public DefaultServerFactory( Object implFactory )
	{
		this.implFactory = implFactory;
	}

	private final Object implFactory;

	public Object sessionQuery( Object query ) throws Exception
	{
		if (implFactory instanceof Session)
			return ((Session) implFactory).sessionQuery( query );

		throw new UnsupportedOperationException( "unknown query " + query );
	}

	public void sessionControl( Object control, Object value ) throws Exception
	{
		if (implFactory instanceof Session)
		{
			((Session) implFactory).sessionControl( control, value );
			return;
		}

		throw new UnsupportedOperationException( "unknown control " + control );
	}

	public void sessionNotify( Object event ) throws Exception
	{
		if (implFactory instanceof Session)
			((Session) implFactory).sessionNotify( event );
		else if (event instanceof Exception)
			throw ((Exception) event);
	}
}
