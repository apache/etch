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

import java.io.EOFException;
import java.net.InetSocketAddress;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;

/**
 * Selector.Handler for a tcp listening port.
 */
public class AcceptHandler extends AbstractHandler<ServerSocketChannel>
{
	/**
	 * @param channel
	 * @param factory 
	 */
	public AcceptHandler( ServerSocketChannel channel, StreamHandlerFactory factory )
	{
		super( channel );
		this.factory = factory;
	}
	
	private final StreamHandlerFactory factory;
	
	@Override
	public String toString()
	{
		return String.format( "AcceptHandler(%s)", channel().socket().getLocalSocketAddress() );
	}
	
	@Override
	protected boolean canAccept()
	{
		return true;
	}

	@Override
	protected final void doAccept() throws Exception
	{
		SocketChannel sc = channel().accept();
		try
		{
			sc.configureBlocking( false );
			acceptedChannel( sc );
		}
		catch ( EOFException e )
		{
			sc.close();
		}
		catch ( Exception e )
		{
			e.printStackTrace();
			sc.close();
		}
	}
	
	protected void acceptedChannel( SocketChannel sc ) throws Exception
	{
		selector().register( factory.newStreamHandler( sc, false ) );
	}

	/**
	 * @return the listening address of this accept handler.
	 */
	public InetSocketAddress getLocalSocketAddress()
	{
		return (InetSocketAddress) channel().socket().getLocalSocketAddress();
	}
}
