/* $Id$
 *
 * Created by wert on Apr 5, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import metreos.core.Who;
import etch.bindings.java.msg.Message;

/**
 * An interface used to deliver responses to a message. Support for
 * the mechanism is somewhat dependent upon properties of the transport
 * and message format.
 */
public interface Mailbox extends MessageHandler
{
	/**
	 * @return the message id of this mailbox.
	 */
	public Long getMessageId();

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
	 */
	public void closeDelivery();
	
	/**
	 * Closes the mailbox so that no more messages will be delivered or
	 * read. Any remaining queued messages are delivered to a default
	 * handler.
	 * @throws Exception 
	 */
	public void closeRead() throws Exception;
	
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
}
