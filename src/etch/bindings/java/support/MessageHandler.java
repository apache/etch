/* $Id$
 *
 * Created by wert on Apr 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import metreos.core.Who;
import etch.bindings.java.msg.Message;

/**
 * Interface used to deliver messages from a message source.
 */
public interface MessageHandler
{
	/**
	 * Delivers a message from a message source.
	 * @param sender the message sender (meaning depends upon the message
	 * source).
	 * @param msg the message from the message source.
	 * @return true if the message was processed.
	 * @throws Exception
	 */
	public boolean message( Who sender, Message msg )
		throws Exception;
}
