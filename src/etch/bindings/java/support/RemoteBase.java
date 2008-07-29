/* $Id$
 *
 * Created by wert on Apr 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.transport.Transport;
import etch.bindings.java.transport.Transport.WaitDown;
import etch.bindings.java.transport.Transport.WaitUp;

/**
 * Base class for call to message translators.
 * @param <T> 
 */
public class RemoteBase<T extends ValueFactory>
{
	/**
	 * Constructs the RemoteBase.
	 *
	 * @param svc
	 * @param vf 
	 */
	public RemoteBase( DeliveryService svc, T vf )
	{
		this._svc = svc;
		this._vf = vf;
	}
	
	/**
	 * The delivery service used to send messages.
	 */
	public final DeliveryService _svc;
	
	/**
	 * The value factory used to create messages and encode/decode types.
	 */
	public final T _vf;
	
	/**
	 * @param type
	 * @return a new message
	 */
	public final Message _newMessage( Type type )
	{
		return new Message( type, _vf );
	}

	/**
	 * Gets a configuration or operational value from the source. The
	 * request is passed down the chain of sources until some source
	 * recognises the query, whereupon it returns the requested value.
	 * 
	 * @param query an object representing a query, which could be as
	 * simple as a string, integer, or enum, or more complex such as
	 * a class with instance variables for query terms.
	 * 
	 * @return the requested value, or null if not defined.
	 * 
	 * @throws UnsupportedOperationException if the query is not recognised
	 * by any source (which is to say, if the last source in the source
	 * chain does not recognise it, it should throw this exception).
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
	 * recognises the control, whereupon it stores the specified value
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
	 * @throws UnsupportedOperationException if the control is not recognised
	 * by any source (which is to say, if the last source in the source
	 * chain does not recognise it, it should throw this exception).
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
		_svc.transportControl( Transport.START, null );
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
		_svc.transportQuery( new WaitUp( maxDelay ) );
	}
	
	/**
	 * Starts the transport and waits for it to come up.
	 * @param maxDelay max delay in milliseconds.
	 * @throws Exception
	 * @see #_transportControl(Object, Object)
	 * @see Transport#START_AND_WAIT_UP
	 */
	public void _startAndWaitUp( int maxDelay ) throws Exception
	{
		_svc.transportControl( Transport.START_AND_WAIT_UP, maxDelay );
	}
	
	/**
	 * Stops the transport.
	 * @throws Exception
	 * @see #_transportControl(Object, Object)
	 * @see Transport#STOP
	 */
	public void _stop() throws Exception
	{
		_svc.transportControl( Transport.STOP, null );
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
		_svc.transportQuery( new WaitDown( maxDelay ) );
	}
	
	/**
	 * Stops the transport and waits for it to go down.
	 * @param maxDelay max delay in milliseconds.
	 * @throws Exception
	 * @see #_transportControl(Object, Object)
	 * @see Transport#STOP_AND_WAIT_DOWN
	 */
	public void _stopAndWaitDown( int maxDelay ) throws Exception
	{
		_svc.transportControl( Transport.STOP_AND_WAIT_DOWN, maxDelay );
	}
}