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

package org.apache.etch.util.core.nio;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

/**
 * Handler for TCP stream channels.
 */
public class StreamHandler extends AbstractHandler<SocketChannel>
{
	/**
	 * Constructs the StreamHandler.
	 * @param channel
	 * @param wantsConnect
	 * @throws IOException
	 */
	public StreamHandler( SocketChannel channel, boolean wantsConnect )
		throws IOException
	{
		super( channel );
		
		this.wantsConnect = wantsConnect;
		
		if (!wantsConnect)
			connected();
	}
	
	private boolean wantsConnect;
	
	@Override
	protected final boolean canConnect()
	{
		return wantsConnect;
	}
	
	@Override
	protected final void doConnect() throws IOException
	{
		wantsConnect = !channel().finishConnect();
		
		if (!wantsConnect)
			connected();
	}
	
	protected void connected() throws IOException
	{
		// nothing to do.
	}

	/**
	 * Reads from the channel of this handler.
	 * @param rbuf the ByteBuffer to read into.
	 * @return the count of bytes read.
	 * @throws IOException
	 */
	public int read( ByteBuffer rbuf ) throws IOException
	{
		return channel().read( rbuf );
	}

	/**
	 * Writes to the channel of this handler.
	 * @param wbuf the ByteBuffer to write from. It is flipped first, and
	 * compacted after.
	 * @return the count of bytes written.
	 * @throws IOException
	 */
	public int write( ByteBuffer wbuf ) throws IOException
	{
		try
		{
			wbuf.flip();
			return wbuf.hasRemaining() ? channel().write( wbuf ) : 0;
		}
		finally
		{
			wbuf.compact();
		}
	}

	/**
	 * Shuts down output on the channel of this handler.
	 * @throws IOException
	 */
	public void shutdownOutput() throws IOException
	{
		channel().socket().shutdownOutput();
	}
}
