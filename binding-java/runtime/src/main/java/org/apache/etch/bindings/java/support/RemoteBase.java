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

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.util.core.io.Transport;
import org.apache.etch.util.core.io.Transport.WaitDown;
import org.apache.etch.util.core.io.Transport.WaitUp;


/**
 * Base class for call to message translators.
 */
public class RemoteBase
{
	/**
	 * Constructs the RemoteBase.
	 *
	 * @param svc the delivery service used to send messages.
	 * @param vf the value factory used to create messages and encode/decode types.
	 */
	public RemoteBase( DeliveryService svc, ValueFactory vf )
	{
		_svc = svc;
		_vf = vf;
	}
	
	private final DeliveryService _svc;
	
	private final ValueFactory _vf;
	
	@Override
	public String toString()
	{
		return String.format( "%s/%s", getClass().getName(), _svc );
	}
	
	/**
	 * Constructs a new message to send using {@link #_send(Message)} or
	 * {@link #_begincall(Message)};
	 * @param type the type of the message.
	 * @return a new message
	 */
	public final Message _newMessage( Type type )
	{
		return new Message( type, _vf );
	}
	
	/**
	 * Sends the message to the recipient, but does not wait for any response.
	 * 
	 * @param msg the message to send
	 * @throws Exception if there is a problem sending
	 */
	public void _send( Message msg ) throws Exception
	{
		_svc.transportMessage( null, msg );
	}
	
	/**
	 * Sends the message which begins a call sequence.
	 * 
	 * @param msg the message to send.
	 * @return a mailbox which can be used to read the response, using
	 * {@link #_endcall(Mailbox, Type)}.
	 */
	public Mailbox _begincall( Message msg )
	{
		try
		{
			return _svc.begincall( msg );
		}
		catch ( RuntimeException e )
		{
			throw e;
		}
		catch ( Exception e )
		{
			throw new RuntimeException( e );
		}
	}

	/**
	 * Finishes a call sequence by waiting for the response message.
	 * 
	 * @param mb a mailbox which will be used to read the response, returned by
	 * {@link #_begincall(Message)}.
	 * @param responseType the type of the expected response.
	 * @return the value of the response field if it isn't an exception.
	 * @throws Exception if there is a problem sending or a timeout waiting or
	 * if the result value was an exception.
	 */
	public Object _endcall( Mailbox mb, Type responseType ) throws Exception
	{
		return _svc.endcall( mb, responseType );
	}

	/**
	 * Gets a configuration or operational value from the source. The
	 * request is passed down the chain of sources until some source
	 * recognizes the query, whereupon it returns the requested value.
	 * 
	 * @param query an object representing a query, which could be as
	 * simple as a string, integer, or enum, or more complex such as
	 * a class with instance variables for query terms.
	 * 
	 * @return the requested value, or null if not defined.
	 * 
	 * @throws UnsupportedOperationException if the query is not recognized
	 * by any source (which is to say, if the last source in the source
	 * chain does not recognize it, it should throw this exception).
	 * 
	 * @throws Exception
	 */
	public Object _transportQuery( Object query ) throws Exception
	{
		return _svc.transportQuery( query );
	}

	/**
	 * Sets a configuration or operational value in the source. The
	 * request is passed down the chain of sources until some source
	 * recognizes the control, whereupon it stores the specified value
	 * and returns.
	 * 
	 * @param control an object representing a control, which could be as
	 * simple as a string, integer, or enum, or more complex such as
	 * a class with instance variables for control terms.
	 * 
	 * @param value the value to set.
	 * 
	 * @throws IllegalArgumentException if the value is not the right
	 * type or if the value is inappropriate.
	 * 
	 * @throws UnsupportedOperationException if the control is not recognized
	 * by any source (which is to say, if the last source in the source
	 * chain does not recognize it, it should throw this exception).
	 * 
	 * @throws Exception
	 */
	public void _transportControl( Object control, Object value ) throws Exception
	{
		_svc.transportControl( control, value );
	}

	/**
	 * Notifies the chain of sources of the specified event. Unlike query
	 * and control operations above, events are always passed down to the
	 * bottom to allow all sources to notice them.
	 * 
	 * @param event a class which represents the event, possibly with
	 * parameters. The simplest event could be a string, integer,
	 * or enum, but any class instance will do (as long as some source
	 * in the chain expects it).
	 * 
	 * @throws Exception
	 */
	public void _transportNotify( Object event ) throws Exception
	{
		_svc.transportNotify( event );
	}

	////////////////////////////
	// Convenience operations //
	////////////////////////////
	
	/**
	 * Starts the transport.
	 * @throws Exception
	 * @see #_transportControl(Object, Object)
	 * @see Transport#START
	 */
	public void _start() throws Exception
	{
		_transportControl( Transport.START, null );
	}
	
	/**
	 * Waits for the transport to come up.
	 * @param maxDelay max delay in milliseconds.
	 * @throws Exception
	 * @see #_transportQuery(Object)
	 * @see WaitUp
	 */
	public void _waitUp( int maxDelay ) throws Exception
	{
		_transportQuery( new WaitUp( maxDelay ) );
	}
	
	/**
	 * Starts the transport and waits for it to come up.
	 * @param maxDelay max delay in milliseconds.
	 * @throws Exception
	 * @see #_start()
	 * @see #_waitUp(int)
	 */
	public void _startAndWaitUp( int maxDelay ) throws Exception
	{
		_start();
		_waitUp( maxDelay );
	}
	
	/**
	 * Stops the transport.
	 * @throws Exception
	 * @see #_transportControl(Object, Object)
	 * @see Transport#STOP
	 */
	public void _stop() throws Exception
	{
		_transportControl( Transport.STOP, null );
	}
	
	/**
	 * Waits for the transport to go down.
	 * @param maxDelay max delay in milliseconds.
	 * @throws Exception
	 * @see #_transportQuery(Object)
	 * @see WaitDown
	 */
	public void _waitDown( int maxDelay ) throws Exception
	{
		_transportQuery( new WaitDown( maxDelay ) );
	}
	
	/**
	 * Stops the transport and waits for it to go down.
	 * @param maxDelay max delay in milliseconds.
	 * @throws Exception
	 * @see #_start()
	 * @see #_waitDown(int)
	 */
	public void _stopAndWaitDown( int maxDelay ) throws Exception
	{
		_stop();
		_waitDown( maxDelay );
	}
}