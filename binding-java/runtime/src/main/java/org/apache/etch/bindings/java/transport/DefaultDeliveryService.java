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

package org.apache.etch.bindings.java.transport;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.Mailbox;
import org.apache.etch.util.Monitor;
import org.apache.etch.util.Resources;
import org.apache.etch.util.TimeoutException;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;


/**
 * Default implementation of DeliveryService.
 */
public class DefaultDeliveryService implements DeliveryService
{
	
	/**
	 * Parameter to Globally Disable Timeout.
	 */
	public final static String DISABLE_TIMEOUT = "DefaultDeliveryService.disableTimeout";
	
	
	private boolean disableTimeout = false;
	
	/**
	 * @param transport
	 * @param uri
	 * @param resources
	 */
	public DefaultDeliveryService( MailboxManager transport, String uri,
		Resources resources )
	{
		this( transport, new URL( uri ), resources );
	}

	/**
	 * @param transport
	 * @param uri
	 * @param resources
	 */
	public DefaultDeliveryService( MailboxManager transport, URL uri,
		Resources resources )
	{
		this.transport = transport;
		transport.setSession( this );
		disableTimeout = uri.getBooleanTerm( DISABLE_TIMEOUT , false );
	}
	
	private final MailboxManager transport;
	
	/**
	 * @return the transport.
	 */
	public MailboxManager getTransport()
	{
		return transport;
	}
	
	public SessionMessage getSession()
	{
		return session;
	}
	
	public void setSession( SessionMessage session )
	{
		if (this.session != null)
			throw new UnsupportedOperationException( "only one stub for now" );
		this.session = session;
	}
	
	private SessionMessage session;

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
		if (event == UP)
			status.set( UP );
		else if (event == DOWN)
			status.set( DOWN );
		
		session.sessionNotify( event );
	}
	
	private final Monitor<String> status = new Monitor<String>( "session status" );

	public boolean sessionMessage( Who sender, Message msg ) throws Exception
	{
		return session.sessionMessage( sender, msg );
	}

	@Override
	public String toString()
	{
		return transport.toString();
	}
	
	private void waitUp( int maxDelay ) throws Exception
	{
		status.waitUntilEq( UP, maxDelay );
	}
	
	private void waitDown( int maxDelay ) throws Exception
	{
		status.waitUntilEq( DOWN, maxDelay );
	}

	public void transportMessage( Who recipient, Message msg ) throws Exception
	{
		transport.transportMessage( recipient, msg );
	}
	
	public Object transportQuery( Object query ) throws Exception
	{
		if (query.getClass() == WaitUp.class)
		{
			waitUp( ((WaitUp) query).maxDelay );
			return null;
		}
		else if (query.getClass() == WaitDown.class)
		{
			waitDown( ((WaitDown) query).maxDelay );
			return null;
		}
		else
		{
			return transport.transportQuery( query );
		}
	}
	
	public void transportControl( Object control, Object value ) throws Exception
	{
		if (control == START_AND_WAIT_UP)
		{
			transport.transportControl( START, null );
			waitUp( (Integer) value );
		}
		else if (control == STOP_AND_WAIT_DOWN)
		{
			transport.transportControl( STOP, null );
			waitDown( (Integer) value );
		}
		else
		{
			transport.transportControl( control, value );
		}
	}

	public void transportNotify( Object event ) throws Exception
	{
		transport.transportNotify( event );
	}

	public Mailbox begincall( Message msg )
	{
		try
		{
			return transport.transportCall( null, msg );
		}
		catch ( Exception e )
		{
			throw new RuntimeException( "unexpected exception sending message", e );
		}
	}

	public Object endcall( Mailbox mb, Type responseType ) throws Exception
	{
		try
		{
			int timeout = disableTimeout ? 0 : responseType.getTimeout();
			Mailbox.Element mbe = mb.read( timeout);
			if (mbe == null)
				throw new TimeoutException( "timeout waiting for "+responseType );
			Message rmsg = mbe.msg;
			rmsg.checkType( responseType );
			Object r = rmsg.get( responseType.getResponseField() );
			if (r instanceof Exception)
			{
				Exception e = (Exception) r;
				e.fillInStackTrace();
				throw e;
			}
			return r;
		}
		finally
		{
			mb.closeRead();
		}
	}
}
