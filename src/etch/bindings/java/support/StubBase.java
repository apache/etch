/* $Id$
 *
 * Created by sccomer on Mar 29, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import metreos.core.Who;
import etch.bindings.java.msg.Message;
import etch.bindings.java.transport.Session;

/**
 * Base class of stub implementations.
 */
public class StubBase implements MessageHandler, Session
{
	/**
	 * Constructs the StubBase.
	 * @param src the message source.
	 * @param obj the target of decoded messages.
	 * @param queued thread pool used to run AsyncReceiverMode.QUEUED methods.
	 * @param free thread pool used to run AsyncReceiverMode.FREE methods.
	 */
	public StubBase( MessageSource src, Object obj, Pool queued, Pool free )
	{
		this._src = src;
		this._obj = obj;
		this._queued = queued;
		this._free = free;
	}
	
	/**
	 * The message source.
	 */
	protected final MessageSource _src;
	
	@SuppressWarnings("unused")
	private final Object _obj;
	
	/**
	 * The thread pool used to run AsyncReceiverMode.QUEUED methods.
	 */
	protected final Pool _queued;
	
	/**
	 * The thread pool used to run AsyncReceiverMode.FREE methods.
	 */
	protected final Pool _free;

	public boolean message( Who sender, Message msg ) throws Exception
	{
		return false;
	}

	public Object sessionQuery( Object query )
	{
		// TODO reflect sessionQuery to service implementation object.
		throw new UnsupportedOperationException( "unknown query "+query );
	}

	public void sessionControl( Object control, Object value )
	{
		// TODO reflect sessionControl to service implementation object.
		throw new UnsupportedOperationException( "unknown control "+control );
	}

	public void sessionNotify( Object event )
	{
		// TODO reflect sessionNotify to service implementation object.
	}
}
