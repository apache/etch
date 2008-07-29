/* $Id$
 *
 * Created by sccomer on Mar 29, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.Type;
import etch.bindings.java.transport.SessionMessage;
import etch.bindings.java.transport.TransportMessage;

/**
 * Adapter between remote and message source.
 */
public interface DeliveryService extends SessionMessage, TransportMessage
{
	/**
	 * Sends the message which begins a call sequence.
	 * 
	 * @param msg the message to send.
	 * @return a mailbox which can be used to read the response, using
	 * {@link #endcall(Mailbox, Type)}.
	 * @throws Exception if there is a problem sending
	 */
	public Mailbox begincall( Message msg ) throws Exception;

	/**
	 * Finishes a call sequence by waiting for the response message.
	 * 
	 * @param mb a mailbox which will be used to read the response, returned by
	 * {@link #begincall(Message)}.
	 * @param responseType the type of the expected response.
	 * @return the value of the response field if it isn't an exception.
	 * @throws Exception if there is a problem sending or a timeout waiting or
	 * if the result value was an exception.
	 */
	public Object endcall( Mailbox mb, Type responseType ) throws Exception;
}
