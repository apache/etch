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

package etch.bindings.java.transport.filters;

import etch.bindings.java.msg.Message;
import etch.bindings.java.transport.SessionMessage;
import etch.bindings.java.transport.TransportMessage;
import etch.util.Resources;
import etch.util.URL;
import etch.util.core.Who;
import etch.util.core.io.Session;

/**
 * AbstractMessageFilter is a useful base class used to implement message filters.
 */
abstract public class AbstractMessageFilter implements TransportMessage, SessionMessage
{
	/**
	 * Constructs the AbstractMessageFilter.
	 * @param transport
	 * @param uri
	 * @param resources
	 */
	protected AbstractMessageFilter( TransportMessage transport, URL uri,
		Resources resources )
	{
		this.transport = transport;
		transport.setSession( this );
	}

	/**
	 * Our transport.
	 */
	protected final TransportMessage transport;
	
	@Override
	abstract public String toString();

	public void transportMessage( Who recipient, Message msg ) throws Exception
	{
		transport.transportMessage( recipient, msg );
	}

	public SessionMessage getSession()
	{
		return session;
	}

	public void setSession( SessionMessage session )
	{
		this.session = session;
	}

	/**
	 * Our session
	 */
	protected SessionMessage session;

	public Object transportQuery( Object query ) throws Exception
	{
		return transport.transportQuery( query );
	}

	public void transportControl( Object control, Object value )
		throws Exception
	{
		transport.transportControl( control, value );
	}

	public void transportNotify( Object event ) throws Exception
	{
		transport.transportNotify( event );
	}

	public boolean sessionMessage( Who sender, Message msg ) throws Exception
	{
		return session.sessionMessage( sender, msg );
	}

	public Object sessionQuery( Object query ) throws Exception
	{
		return session.sessionQuery( query );
	}

	public void sessionControl( Object control, Object value ) throws Exception
	{
		session.sessionControl( control, value );
	}

	public void sessionNotify( Object event ) throws Exception
	{
		if (event == Session.UP)
			if (!sessionUp())
				return;

		if (event == Session.DOWN)
			if (!sessionDown())
				return;

		session.sessionNotify( event );
	}

	/**
	 * Tells the filter that the session is up.
	 * @return true if the event should be passed on to our session, false
	 * if it should be swallowed.
	 * @throws Exception
	 */
	protected boolean sessionUp() throws Exception
	{
		// nothing to do.
		return true;
	}

	/**
	 * Tells the filter that the session is down.
	 * @return true if the event should be passed on to our session, false
	 * if it should be swallowed.
	 * @throws Exception
	 */
	protected boolean sessionDown() throws Exception
	{
		// nothing to do.
		return true;
	}
}
