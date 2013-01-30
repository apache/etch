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
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;
import java.util.LinkedList;

import org.apache.etch.util.AlarmListener;
import org.apache.etch.util.AlarmManager;
import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.Monitor;
import org.apache.etch.util.Resources;
import org.apache.etch.util.Todo;
import org.apache.etch.util.TodoManager;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.SessionData;
import org.apache.etch.util.core.io.TcpConnection;
import org.apache.etch.util.core.io.TcpOptions;
import org.apache.etch.util.core.io.TransportData;


/**
 * A TCP-based transport based on selectors.
 */
public class Tcp2Connection implements TransportData, StreamHandlerFactory, AlarmListener
{
	/**
	 * @param uri
	 * @param resources
	 */
	public Tcp2Connection( String uri, Resources resources )
	{
		this( new URL( uri ), resources );
	}

	/**
	 * @param uri
	 * @param resources
	 */
	public Tcp2Connection( URL uri, Resources resources )
	{
		selector = (SuperSelector) resources.get( "selector" );
		bufferPool = (ByteBufferPool) resources.get( "bufferPool" );
		options = new TcpOptions( uri, resources );

		connection = (SocketChannel) resources.remove( "connection" );

		if (connection == null)
		{
			String h = uri.getHost();
			if (h == null)
				throw new IllegalArgumentException( "host == null" );

			Integer p = uri.getPort();
			if (p == null)
				throw new IllegalArgumentException( "port == null" );

			if (p <= 0 || p > 65535)
				throw new IllegalArgumentException( "port <= 0 || port > 65535" );

			host = h;
			port = p;
		}
		else
		{
			host = null;
			port = 0;
		}
	}

	private final SuperSelector selector;
	
	private final ByteBufferPool bufferPool;

	private final TcpOptions options;

	private SocketChannel connection;

	private final String host;

	private final int port;

	@Override
	public String toString()
	{
		try
		{
			Socket s = checkSocket();
			return String.format( "Tcp2Connection(up, %s:%d, %s:%d)", s
				.getLocalAddress(), s.getLocalPort(), s.getInetAddress(), s
				.getPort() );
		}
		catch ( IOException e )
		{
			return String.format( "Tcp2Connection(down, %s:%d)", host, port );
		}
	}

	protected boolean isServer()
	{
		return host == null;
	}

	// ///////////
	// SESSION //
	// ///////////

	public SessionData getSession()
	{
		return session;
	}

	public void setSession( SessionData session )
	{
		this.session = session;
	}

	private SessionData session;

	// /////////////
	// TRANSPORT //
	// /////////////

	public Object transportQuery( Object query ) throws Exception
	{
		if (query == IS_SERVER)
			return isServer();

		if (query == TcpConnection.LOCAL_ADDRESS)
			return localAddress();

		if (query == TcpConnection.REMOTE_ADDRESS)
			return remoteAddress();

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

		if (control == RESET)
		{
			close( true );
			return;
		}

		throw new UnsupportedOperationException( "unknown control: " + control );
	}

	public void transportNotify( Object event ) throws Exception
	{
		// nothing to do.
	}

	public void transportData( Who recipient, FlexBuffer buf ) throws Exception
	{
		int n = buf.avail();
		if (n == 0)
			return;
		
		MyStreamHandler h = checkHandler();

		// TODO better job with buffers
		
		ByteBuffer bb = ByteBuffer.allocateDirect( n );
		bb.put( buf.getBuf(), buf.index(), n );
		
		bb.flip();
		h.writeBuf( bb );
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
		if (host != null)
		{
//			MyStreamHandler h = (MyStreamHandler)
			selector.newStreamHandler( new InetSocketAddress( host, port ), this );
			//setHandler( h ); // fireUp will do it
			return;
		}

		SocketChannel c = connection;
		if (c != null)
		{
			connection = null;
//			MyStreamHandler h = (MyStreamHandler)
			selector.newStreamHandler( c, this );
			//setHandler( h ); // fireUp will do it
			return;
		}

		throw new IOException( "cannot start, no connection" );
	}

	private boolean started;
	
	private final Object startedSync = new Object();

	private MyStreamHandler setHandler( MyStreamHandler newHandler )
	{
		synchronized (handlerSync)
		{
			if (newHandler == handler)
				return null;
			
			if (newHandler != null && handler != null)
				throw new IllegalStateException(
					"newHandler != null && handler != null && newHandler != handler" );
			
			MyStreamHandler oldHandler = handler;
			handler = newHandler;
			return oldHandler;
		}
	}

	private MyStreamHandler handler;
	
	private final Object handlerSync = new Object();

	void stop() throws IOException
	{
		started = false;
		
		MyStreamHandler h = setHandler( null );
		if (h != null)
		{
			h.shutdownOutput();
			h.cancel();
		}
	}

	void close( boolean reset )
	{
		MyStreamHandler h = setHandler( null );
		if (h != null)
		{
			if (reset)
			{
				try
				{
					h.setLinger( false, 0 );
				}
				catch ( Exception e )
				{
					e.printStackTrace();
				}
			}
			h.cancel();
		}
	}

	private void fireUp( MyStreamHandler h )
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
		
		if (started && host != null && options.reconnectDelay > 0)
			AlarmManager.staticAdd( this, 0, options.reconnectDelay );
		
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

	private void fireException( String what, final Exception e )
	{
		TodoManager.addTodo( new Todo()
		{
			public void doit( TodoManager m ) throws Exception
			{
				session.sessionNotify( e );
			}

			public void exception( TodoManager m, Exception ex )
			{
				e.printStackTrace();
				if (ex != e)
					ex.printStackTrace();
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

	SocketAddress remoteAddress() throws IOException
	{
		return checkSocket().getRemoteSocketAddress();
	}

	SocketAddress localAddress() throws IOException
	{
		return checkSocket().getLocalSocketAddress();
	}

	private Socket checkSocket() throws IOException
	{
		Socket s = checkChannel().socket();
		if (s == null)
			throw new IOException( "no socket" );
		return s;
	}

	private SocketChannel checkChannel() throws IOException
	{
		SocketChannel c = checkHandler().channel();
		if (c == null)
			throw new IOException( "no channel" );
		return c;
	}

	private MyStreamHandler checkHandler() throws IOException
	{
		MyStreamHandler h = handler;
		if (h == null)
			throw new IOException( "no handler" );
		return h;
	}

	public MyStreamHandler newStreamHandler( SocketChannel channel,
		boolean wantsConnect ) throws Exception
	{
		return new MyStreamHandler( channel, wantsConnect );
	}

	class MyStreamHandler extends StreamHandler
	{
		public MyStreamHandler( SocketChannel channel, boolean wantsConnect )
			throws IOException
		{
			super( channel, wantsConnect );
		}

		@Override
		protected void connected() throws IOException
		{
			super.connected();
			
			Socket s = channel().socket();

			s.setKeepAlive( options.keepAlive );
			s.setSoLinger( options.lingerTime >= 0,
				options.lingerTime >= 0 ? options.lingerTime : 0 );
			s.setTcpNoDelay( options.noDelay );
			s.setTrafficClass( options.trafficClass );

			fireUp( this );
		}

		public void setLinger( boolean on, int linger ) throws IOException
		{
			Socket s = channel().socket();
			s.setSoLinger( on, linger );
		}

		public void writeBuf( ByteBuffer bb ) throws IOException
		{
			synchronized (wlist)
			{
				if (!bb.hasRemaining())
					return;

				if (!wlist.isEmpty())
				{
					wlist.add( bb );
					return;
				}

				channel().write( bb );
				if (!bb.hasRemaining())
					return;

				// there's some leftovers...

				wlist.add( bb );
				updateInterestOps();
			}
		}
		
		@Override
		protected boolean canRead() throws Exception
		{
			return true;
		}
		
		@Override
		protected void doRead() throws Exception
		{
			ByteBuffer buf = bufferPool.alloc( null );
			try
			{
				int n = read( buf );
				if (n <= 0)
					throw new EOFException( "n <= 0" );
				
				buf.flip();
				byte[] bbuf = new byte[buf.remaining()];
				buf.get( bbuf );
				
				FlexBuffer fbuf = new FlexBuffer( bbuf );
				session.sessionData( null, fbuf );
			}
			finally
			{
				if (buf != null)
					bufferPool.release( buf );
			}
			
		}

		@Override
		protected boolean canWrite() throws Exception
		{
			return !wlist.isEmpty();
		}

		private final LinkedList<ByteBuffer> wlist = new LinkedList<ByteBuffer>();

		@Override
		protected void doWrite() throws Exception
		{
			synchronized (wlist)
			{
				ByteBuffer buf = wlist.getFirst();
				write( buf );
				if (!buf.hasRemaining())
					wlist.removeFirst();
			}
		}

		@Override
		public int write( ByteBuffer wbuf ) throws IOException
		{
			throw new IOException( "don't use this!" );
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

	void close()
	{
		close( false );
	}

	/**
	 * @throws IOException
	 */
	public void shutdownOutput() throws IOException
	{
		StreamHandler h = handler;
		if (h != null)
			h.shutdownOutput();
	}

	public int wakeup( AlarmManager manager, Object state, long due )
	{
		try
		{
			restart();
			return 0;
		}
		catch ( Exception e )
		{
			fireException( "reconnect", e );
			return started ? options.reconnectDelay : 0;
		}
	}
}
