/* $Id$
 *
 * Created by sccomer on Mar 29, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.Type;
import etch.bindings.java.transport.Transport;

/**
 * Adapter between remote and message source.
 */
public interface DeliveryService extends Transport
{
	/**
	 * Sends the message to the recipient, but does not wait for
	 * any response.
	 * @param msg the message to send
	 * @throws Exception if there is a problem sending
	 */
	public void send( Message msg ) throws Exception;
	
	/**
	 * Sends the message which begins a call sequence.
	 * @param msg the message to send.
	 * @return a mailbox which can be used to read response,
	 * using either {@link #endvoidcall(Mailbox, Type, Field, int)}
	 * or {@link #endcall(Mailbox, Type, Field, int)}.
	 */
	public Mailbox begincall( Message msg );

	/**
	 * @param mb the mailbox returned by {@link #begincall(Message)}.
	 * @param responseType the type of the expected response.
	 * @param responseField the field of the expected response
	 * which would contain any result value or thrown exception.
	 * @param timeout time in ms to wait for a response.
	 * @throws Exception if there is a problem sending or a timeout
	 * waiting or if the result value was an exception.
	 */
	public void endvoidcall( Mailbox mb, Type responseType,
		Field responseField, int timeout ) throws Exception;

	/**
	 * @param mb
	 * @param responseType the type of the expected response.
	 * @param responseField the field of the expected response
	 * which would contain any result value or thrown exception.
	 * @param timeout time in ms to wait for a response.
	 * @return the value of the response field if it isn't an exception.
	 * @throws Exception if there is a problem sending or a timeout
	 * waiting or if the result value was an exception.
	 */
	public Object endcall( Mailbox mb, Type responseType,
		Field responseField, int timeout ) throws Exception;
}
