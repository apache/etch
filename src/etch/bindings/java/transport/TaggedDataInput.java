/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport;

import java.io.IOException;

import etch.bindings.java.msg.Message;
import etch.util.FlexBuffer;

/**
 * A TaggedDataInput reads type tagged values from an input buf.
 */
public interface TaggedDataInput
{
	/**
	 * Reads a message from the buf.
	 * @param buf the flex buffer containing the message.
	 * @return a message read from the buf.
	 * @throws IOException 
	 */
	public Message readMessage( FlexBuffer buf ) throws IOException;
}
