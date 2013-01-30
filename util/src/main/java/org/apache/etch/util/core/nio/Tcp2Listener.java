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
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.SocketAddress;
import java.net.SocketException;
import java.nio.channels.ServerSocketChannel;
import java.nio.channels.SocketChannel;

import org.apache.etch.util.Monitor;
import org.apache.etch.util.Resources;
import org.apache.etch.util.Todo;
import org.apache.etch.util.TodoManager;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.SessionListener;
import org.apache.etch.util.core.io.TcpConnection;
import org.apache.etch.util.core.io.Transport;


/**
 * Selector based TCP listener.
 * 
 */
public class Tcp2Listener implements Transport<SessionListener<SocketChannel>>,
	AcceptHandlerFactory
{
	/**
	 * @param uri
	 * @param resources
	 */
	public Tcp2Listener( String uri, Resources resources )
	{
		this( new URL( uri ), resources );
	}

	/**
	 * @param uri
	 * @param resources
	 */
	public Tcp2Listener( URL uri, Resources resources )
	{
		this( uri.getHost(), uri.getPort(), uri
			.getIntegerTerm( "backlog", 0 ), resources );
	}

	private Tcp2Listener( String host, Integer port, int backlog,
		Resources resources )
	{
		if (port == null)
			throw new IllegalArgumentException( "port not specified" );
		
		if (port < 0 || port > 65535)
			throw new IllegalArgumentException( "port < 0 || port > 65535" );
		
		if (backlog < 0)
			throw new IllegalArgumentException( "backlog < 0" );

		selector = (SuperSelector) resources.get( "selector" );

		this.host = host;
		this.port = port;
		this.backlog = backlog;
	}

	private final SuperSelector selector;

	private final String host;

	private final int port;

	private final int backlog;

	/**
	 * Host name to specify to select listening on all interfaces. The value is
	 * "0.0.0.0".
	 */
	public static final String ALL_INTFS = "0.0.0.0";

	/**
	 * @param s
	 * @return null if s is null or ALL_INTFS ("0.0.0.0").
	 */
	protected static String translateHost( String s )
	{
		if (s != null && s.equals( ALL_INTFS ))
			return null;
		return s;
	}

	@Override
	public String toString()
	{
		ServerSocket s = null;

		if (s != null)
			return String.format( "Tcp2Listener(up, %s, %d)", s
				.getInetAddress(), s.getLocalPort() );

		return String.format( "Tcp2Listener(down, %s, %d)", host, port );
	}

	// ///////////
	// SESSION //
	// ///////////

	public SessionListener<SocketChannel> getSession()
	{
		return session;
	}

	public void setSession( SessionListener<SocketChannel> session )
	{
		this.session = session;
	}

	private SessionListener<SocketChannel> session;

	// /////////////
	// TRANSPORT //
	// /////////////

	public Object transportQuery( Object query ) throws Exception
	{
		if (query == TcpConnection.LOCAL_ADDRESS)
			return localAddress();

		if (query instanceof WaitUp)
		{
			waitUp( ((WaitUp) query).maxDelay );
			return null;
		}

		if (query instanceof WaitDown)
		{
			waitDown( ((WaitDown) query).maxDelay );
			return null;
		}

		throw new UnsupportedOperationException( "unknown query: " + query );
	}

	public void transportControl( Object control, Object value )
		throws Exception
	{
		if (control == START)
		{
			start();
			return;
		}

		if (control == START_AND_WAIT_UP)
		{
			start();
			waitUp( (Integer) value );
			return;
		}

		if (control == STOP)
		{
			stop();
			return;
		}

		if (control == STOP_AND_WAIT_DOWN)
		{
			stop();
			waitDown( (Integer) value );
			return;
		}

		throw new UnsupportedOperationException( "unknown control: " + control );
	}

	public void transportNotify( Object event ) throws Exception
	{
		// nothing to do.
	}

	// //////////////////
	// IMPLEMENTATION //
	// //////////////////

	void start() throws Exception
	{
		synchronized (startedSync)
		{
			if (started)
				throw new IllegalStateException( "started" );
			
			started = true;
			restart();
		}
	}
	
	private void restart() throws Exception
	{
		// fireUp will save the handler.
		try
		{
			selector.newAcceptHandler(
				new InetSocketAddress( host, port ), backlog, this );
		}
		catch ( Error e )
		{
			// This is a hack work around for jdk_1.5.0_15, which has no
			// translation for the error message and turns it into an
			// Error.
			
			String msg = e.getMessage();
			if (msg == null || !msg.equals( "Untranslated exception" ))
				throw e;
			
			Throwable x = e.getCause();
			if (x == null || !(x instanceof SocketException))
				throw e;
			
			throw (SocketException) x;
		}
	}

	private boolean started;
	
	private final Object startedSync = new Object();
	
	private MyAcceptHandler handler;
	
	private final Object handlerSync = new Object();

	void stop() throws IOException
	{
		started = false;
		
		MyAcceptHandler h = setHandler( null );
		if (h != null)
		{
			h.cancel();
		}
	}

	private MyAcceptHandler setHandler( MyAcceptHandler newHandler )
	{
		synchronized (handlerSync)
		{
			if (newHandler == handler)
				return null;
			
			if (newHandler != null && handler != null)
				throw new IllegalStateException(
					"newHandler != null && handler != null && newHandler != handler" );
			
			MyAcceptHandler oldHandler = handler;
			handler = newHandler;
			return oldHandler;
		}
	}

	private void fireUp( MyAcceptHandler h )
	{
		setHandler( h );
		status.set( Session.UP );
		
		TodoManager.addTodo( new Todo()
		{
			public void doit( TodoManager m ) throws Exception
			{
				session.sessionNotify( Session.UP );
			}

			public void exception( TodoManager m, Exception e )
			{
				e.printStackTrace();
			}
		} );
	}

	private void fireDown()
	{
		setHandler( null );
		status.set( Session.DOWN );
		
		TodoManager.addTodo( new Todo()
		{
			public void doit( TodoManager m ) throws Exception
			{
				session.sessionNotify( Session.DOWN );
			}

			public void exception( TodoManager m, Exception e )
			{
				e.printStackTrace();
			}
		} );
	}

	void waitUp( int maxDelay ) throws Exception
	{
		status.waitUntilEq( Session.UP, maxDelay );
	}

	void waitDown( int maxDelay ) throws Exception
	{
		status.waitUntilEq( Session.DOWN, maxDelay );
	}

	private final Monitor<String> status = new Monitor<String>( "status",
		Session.DOWN );

	SocketAddress localAddress() throws IOException
	{
		return checkSocket().getLocalSocketAddress();
	}

	private ServerSocket checkSocket() throws IOException
	{
		ServerSocket s = checkChannel().socket();
		if (s == null)
			throw new IOException( "no socket" );
		return s;
	}

	private ServerSocketChannel checkChannel() throws IOException
	{
		ServerSocketChannel c = checkHandler().channel();
		if (c == null)
			throw new IOException( "c == null" );
		return c;
	}

	private AcceptHandler checkHandler() throws IOException
	{
		AcceptHandler h = handler;
		if (h == null)
			throw new IOException( "h == null" );
		return h;
	}

	public AcceptHandler newAcceptHandler( ServerSocketChannel channel )
		throws Exception
	{
		return new MyAcceptHandler( channel, null );
	}
	
	/** */
	public class MyAcceptHandler extends AcceptHandler
	{
		/**
		 * @param channel
		 * @param factory
		 */
		public MyAcceptHandler( ServerSocketChannel channel,
			StreamHandlerFactory factory )
		{
			super( channel, factory );
		}
		
		@Override
		protected void registered()
		{
			super.registered();
			fireUp( this );
		}
		
		@Override
		protected void acceptedChannel( SocketChannel sc ) throws Exception
		{
			session.sessionAccepted( sc );
		}
		
		@Override
		public void canceled( Exception e )
		{
			super.canceled( e );
			fireDown();
		}
	}

	boolean isStarted()
	{
		return started;
	}

	/**
	 * @return the host interface to listen on.
	 */
	public String getHost()
	{
		return host;
	}

	/**
	 * @return the port to listen on.
	 */
	public int getPort()
	{
		return port;
	}

	/**
	 * @return the backlog (the number of unaccepted connections to allow).
	 */
	public int getBacklog()
	{
		return backlog;
	}
}
