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
import java.io.IOException;
import java.nio.channels.SelectableChannel;
import java.nio.channels.SelectionKey;

/**
 * Abstract implementation of Handler.
 * @param <T>
 */
public class AbstractHandler<T extends SelectableChannel>
	implements Handler<T>
{
	/**
	 * @param channel
	 */
	public AbstractHandler( T channel )
	{
		this.channel = channel;
	}
	
	private final T channel;

	public final T channel()
	{
		return channel;
	}

	public final void registered( Selector selector, SelectionKey key )
	{
		this.selector = selector;
		this.key = key;
		registered();
	}

	private Selector selector;
	
	private SelectionKey key;
	
	/**
	 * @return the Selector of the handler.
	 */
	public final Selector selector()
	{
		return selector;
	}

	public final SelectionKey key()
	{
		return key;
	}
	
	/**
	 * Notifies the handler that it has been registered with a selector.
	 */
	protected void registered()
	{
		// nothing to do.
	}

	public final int getLastInterestOps()
	{
		return lastInterestOps;
	}

	public final void setLastInterestOps( int ops )
	{
		lastInterestOps = ops;
	}

	private int lastInterestOps;
	
	public final void selected() throws Exception
	{
		SelectionKey k = key;
		if (k == null)
			return;
		
		int ops = k.readyOps();
		
		if ((ops & SelectionKey.OP_ACCEPT) != 0)
		{
			doAccept();
			return;
		}
		
		if ((ops & SelectionKey.OP_CONNECT) != 0)
		{
			doConnect();
			return;
		}
		
		if ((ops & SelectionKey.OP_READ) != 0)
			doRead();
		
		if ((ops & SelectionKey.OP_WRITE) != 0)
			doWrite();
	}

	protected void doAccept() throws Exception
	{
		throw new IOException( "doAccept not implemented" );
	}

	protected void doConnect() throws Exception
	{
		throw new IOException( "doConnect not implemented" );
	}

	protected void doRead() throws Exception
	{
		throw new IOException( "doRead not implemented" );
	}

	protected void doWrite() throws Exception
	{
		throw new IOException( "doWrite not implemented" );
	}

	public final int getInterestOps() throws Exception
	{
		if (canAccept())
			return SelectionKey.OP_ACCEPT;
		
		if (canConnect())
			return SelectionKey.OP_CONNECT;
		
		int ops = canRead() ? SelectionKey.OP_READ : 0;
		
		if (canWrite())
			ops |= SelectionKey.OP_WRITE;
		
		return ops;
	}
	
	/**
	 * @return true if this handler can accept. Called from
	 * {@link #getInterestOps()}.
	 * @throws Exception 
	 */
	protected boolean canAccept() throws Exception
	{
		return false;
	}
	
	/**
	 * @return true if this handler can connect. Called from
	 * {@link #getInterestOps()}.
	 * @throws Exception 
	 */
	protected boolean canConnect() throws Exception
	{
		return false;
	}
	
	/**
	 * @return true if this handler can read. Called from
	 * {@link #getInterestOps()}.
	 * @throws Exception 
	 */
	protected boolean canRead() throws Exception
	{
		return false;
	}
	
	/**
	 * @return true if this handler can write. Called from
	 * {@link #getInterestOps()}.
	 * @throws Exception 
	 */
	protected boolean canWrite() throws Exception
	{
		return false;
	}

	/**
	 * Updates the interest ops of the channel.
	 */
	public final void updateInterestOps()
	{
		Selector s = selector;
		if (s == null)
			throw new IllegalStateException( "selector == null" );
		s.updateInterestOps( this );
	}
	
	/**
	 * Cancels the registration of this handler.
	 */
	public final void cancel()
	{
		Selector s = selector;
		if (s != null)
			s.cancel( this );
		else
			canceled( null );
	}
	
	public void canceled( Exception e )
	{
		try
		{
			if (e != null)
				reportException( e );
			
			if (key != null)
				key.cancel();
			
			channel.close();
		}
		catch ( Exception e1 )
		{
			reportException( e1 );
		}
		finally
		{
			selector = null;
			key = null;
		}
	}

	public void reportException( Exception e )
	{
		if (e instanceof IOException)
		{
			if (e instanceof EOFException)
				return;
			
			String msg = e.getMessage();
			if (msg != null)
			{
				if (msg.equals( "An existing connection was forcibly closed by the remote host" ))
					return;
			}
		}
		
		System.err.printf( "%s: caught exception: %s\n", this, e );
		e.printStackTrace();
	}
}
