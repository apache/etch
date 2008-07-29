// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package etch.bindings.java.transport;

import metreos.core.Who;
import metreos.core.io.Source;
import etch.bindings.java.msg.Message;
import etch.bindings.java.support.Mailbox;
import etch.bindings.java.support.MessageSource;

/**
 * Interface used to send messages back to a message source.
 */
public interface MailboxMessageSource
	extends MessageSource,
		Source<MessageHandler<MailboxMessageSource>>
{
	/**
	 * Sends a message to a message source. The message is encoded
	 * in some way (e.g., compressed binary format), and then delivered
	 * to a transport (e.g., packetized data stream).
	 * @param recipient a transport specific opaque value which identifies
	 * the sender to a message handler and may be passed here to send a
	 * message back. Passing null means "use the default recipient".
	 * @param msg the message to send.
	 * @param maxMessages maximum number of messages which can be queued
	 * to the mailbox before it is full.
	 * @param lifetime time in ms until mailbox expires and is closed. 0
	 * means don't expire the mailbox.
	 * @return a mailbox created to receive replies to this message.
	 * @throws Exception
	 */
	public Mailbox message( Who recipient, Message msg, int maxMessages,
		int lifetime ) throws Exception;
}
