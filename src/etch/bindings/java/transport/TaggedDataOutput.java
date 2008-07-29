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
 * A TaggedDataOutput writes type tagged data values to an output buf.
 */
public interface TaggedDataOutput
{
	/**
	 * Writes the message to the buf.
	 * @param msg the message to be written.
	 * @param buf the buffer to write to.
	 * @throws IOException
	 */
	public void writeMessage( Message msg, FlexBuffer buf ) throws IOException;
}
