/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport;

import java.util.concurrent.TimeoutException;

import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.DeliveryService;
import etch.bindings.java.support.Mailbox;
import etch.util.Monitor;
import etch.util.Resources;
import etch.util.core.Who;

/**
 * Default implementation of DeliveryService.
 */
public class DefaultDeliveryService implements DeliveryService
{
	/**
	 * @param transport
	 * @param uri 
	 * @param resources 
	 */
	public DefaultDeliveryService( MailboxManager transport, String uri,
		Resources resources )
	{
		this.transport = transport;
		transport.setSession( this );
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
			Mailbox.Element mbe = mb.read( responseType.getTimeout() );
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
			mb.closeDelivery();
		}
	}
}
