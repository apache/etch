/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.bindings.java.support;

import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.Transport;

/**
 * Default implementation of ServerFactory. Used by Etch generated Helper files
 * to provide listeners with backstop implementations of Session methods which
 * forward to user's implementation factory.
 */
abstract public class DefaultServerFactory implements ServerFactory
{
	/**
	 * Constructs the DefaultServerFactory.
	 * @param listener 
	 * @param implFactory
	 */
	public DefaultServerFactory( Transport<ServerFactory> listener,
		Object implFactory )
	{
		this.listener = listener;
		
		if (implFactory instanceof Session)
			setSession( (Session) implFactory );
		
		listener.setSession( this );
	}
	
	private final Transport<ServerFactory> listener;

	public Object sessionQuery( Object query ) throws Exception
	{
		if (session != null)
			return session.sessionQuery( query );

		throw new UnsupportedOperationException( "unknown query " + query );
	}

	public void sessionControl( Object control, Object value ) throws Exception
	{
		if (session != null)
		{
			session.sessionControl( control, value );
			return;
		}

		throw new UnsupportedOperationException( "unknown control " + control );
	}

	public void sessionNotify( Object event ) throws Exception
	{
		if (session != null)
			session.sessionNotify( event );
	}

	public Session getSession()
	{
		return session;
	}

	public void setSession( Session session )
	{
		this.session = session;
	}
	
	private Session session;

	public void transportControl( Object control, Object value )
		throws Exception
	{
		listener.transportControl( control, value );
	}

	public void transportNotify( Object event ) throws Exception
	{
		listener.transportNotify( event );
	}

	public Object transportQuery( Object query ) throws Exception
	{
		return listener.transportQuery( query );
	}
}
