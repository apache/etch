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
import org.apache.etch.util.core.Who;


/**
 * An interface used to deliver responses to a message. Support for
 * the mechanism is somewhat dependent upon properties of the transport
 * and message format.
 */
public interface Mailbox
{
	/**
	 * @return the message id of this mailbox.
	 */
	public Long getMessageId();

	/**
	 * Queues the message to the mailbox.
	 * @param sender
	 * @param msg
	 * @return true if the message was accepted.
	 * @throws Exception
	 */
	public boolean message( Who sender, Message msg ) throws Exception;

	/**
	 * @return the next message to be read from the mailbox, or null if
	 * the mailbox is empty and closed. Wait forever for such a message
	 * to be delivered.
	 * @throws InterruptedException thread interrupt.
	 */
	public Element read() throws InterruptedException;

	/**
	 * @param maxDelay the maximum amount of time in milliseconds to
	 * wait to read a message from an empty mailbox. 0 means wait
	 * forever, -1 means don't wait at all.
	 * @return the message read from the mailbox, or null if the mailbox
	 * is empty and closed, or if the time limit was exceeeded.
	 * @throws InterruptedException thread interrupt.
	 */
	public Element read( int maxDelay ) throws InterruptedException;
	
	/**
	 * Closes the mailbox so that no more messages can be delivered.
	 * Queued messages remain to be read. Reading an empty closed
	 * mailbox returns null.
	 * @return true if this call closed the mailbox (that is, if action was
	 * taken), false if the mailbox was already closed.
	 */
	public boolean closeDelivery();
	
	/**
	 * Closes the mailbox so that no more messages will be delivered or
	 * read. Any remaining queued messages are delivered to a default
	 * handler.
	 * @return true if this call closed the mailbox (that is, if action was
	 * taken), false if the mailbox was already closed.
	 * @throws Exception
	 */
	public boolean closeRead() throws Exception;
	
	/**
	 * Registers a Notify interface implementation to receive a callback
	 * when a mailbox's status is changed.
	 * @param notify a Notify interface implementation to report the
	 * delivery status to.
	 * @param state a state value to pass thru to the Notify interface
	 * implementation.
	 * @param maxDelay the maximum amount of time in milliseconds to
	 * wait for delivery of a message to the mailbox. 0 means wait
	 * forever. The mailbox is closed upon timeout.
	 * @throws IllegalStateException notify already registered.
	 */
	public void registerNotify( Notify notify, Object state, int maxDelay );
	
	/**
	 * Unregisters a Notify interface implementation from receiving a callback
	 * when a mailbox's status is changed. Cancels any timeout.
	 * @param notify a Notify interface implementation which was previously
	 * registered.
	 * @throws IllegalStateException the specified notify is not registered.
	 */
	public void unregisterNotify( Notify notify );

	/**
	 * @return true if the mailbox is empty.
	 */
	public boolean isEmpty();
	
	/**
	 * @return true if the mailbox is closed.
	 */
	public boolean isClosed();

	/**
	 * @return true if the mailbox is full.
	 */
	public boolean isFull();
	
	/**
	 * The message as queued, including src and sender.
	 */
	public final class Element
	{
		/**
		 * @param sender the message sender.
		 * @param msg the message.
		 */
		public Element( Who sender, Message msg )
		{
			this.sender = sender;
			this.msg = msg;
		}
		
		/**
		 * The message sender.
		 */
		
		public final Who sender;
		/**
		 * the message.
		 */
		public final Message msg;
	}
	
	/**
	 * Interface used to notify of mailbox status.
	 */
	public interface Notify
	{
		/**
		 * Notifies of mailbox status change.
		 * @param mb the mailbox whose status has changed.
		 * @param state the state object passed in the register
		 * method.
		 * @param closed true if the mailbox timeout has expired and
		 * the mailbox is now closed to delivery, false if a message
		 * has arrived.
		 */
		public void mailboxStatus( Mailbox mb, Object state, boolean closed );
	}
}
