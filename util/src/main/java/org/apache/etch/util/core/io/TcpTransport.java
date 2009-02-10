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

package org.apache.etch.util.core.io;

import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;

import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;


/**
 * Constants and methods related to tcp transport.
 */
abstract public class TcpTransport extends Connection<SessionData>
	implements TransportData
{
	/**
	 * Constructs the TcpTransport. Pulls common parameters off the uri.
	 *
	 * @param uri
	 * @param resources
	 */
	protected TcpTransport( URL uri, Resources resources )
	{
		options = new TcpOptions( uri, resources );
	}
	
	private final TcpOptions options;
	
	@Override
	protected final void stop0() throws Exception
	{
		try
		{
			close( false );
		}
		catch ( Exception e )
		{
			// ignore
		}
		super.stop0();
	}

	/**
	 * The socket for this connection.
	 */
	protected Socket socket;

	/**
	 * @return the socket for this connection if it is good.
	 * @throws SocketException if the socket is bad.
	 */
	protected final Socket checkSocket() throws SocketException
	{
		Socket s = socket;
		
		if (s == null)
			throw new SocketException( "socket closed" );
		
		if (s.isClosed())
			throw new SocketException( "socket closed" );
		
		return s;
	}
	
	@Override
	public final void close( boolean reset )
	{
		// Log.report( "close" );

		Socket s = socket;
		if (s != null)
		{
			try
			{
				try
				{
					if (reset)
					{
						s.setSoLinger( false, 0 );
					}
					else
					{
						flush();
						shutdownOutput();
					}
				}
				finally
				{
					// Log.report( "closing" );
					s.close();
					// Log.report( "closed" );
				}
			}
			catch ( IOException e )
			{
				// ignore.
			}
			finally
			{
				inputStream = null;
				outputStream = null;
				socket = null;
			}
		}
	}

	/**
	 * Sends some data to the remote end. If the connection is buffered, the
	 * data is saved until the buffer is full or the buffer is flushed. If the
	 * autoFlush setting is true, send automatically flushes the data. If the
	 * connection is not buffered, the data is sent to the operating system
	 * immediately. The operating system may or may not transmit the data
	 * immediately, depending upon the noDelay setting.
	 * 
	 * @param buf the bytes to be sent.
	 * @throws Exception if there is a problem transmitting the data. Such a
	 * problem causes the current connection to be reset.
	 * @see #flush()
	 * @see TcpOptions#AUTO_FLUSH
	 * @see TcpOptions#BUFFER_SIZE
	 * @see TcpOptions#NO_DELAY
	 */
	public final void send( byte[] buf ) throws Exception
	{
		send( buf, 0, buf.length );
	}

	/**
	 * Sends some data to the remote end. If the connection is buffered, the
	 * data is saved until the buffer is full or the buffer is flushed. If the
	 * autoFlush setting is true, send automatically flushes the data. If the
	 * connection is not buffered, the data is sent to the operating system
	 * immediately. The operating system may or may not transmit the data
	 * immediately, depending upon the noDelay setting.
	 * @param buf the bytes to be sent.
	 * @param off the offset into buf of the first byte to send.
	 * @param len the number of bytes to send.
	 * @throws Exception if there is a problem transmitting the data. Such a
	 * problem causes the current connection to be reset.
	 * @see #flush()
	 * @see TcpOptions#AUTO_FLUSH
	 * @see TcpOptions#BUFFER_SIZE
	 * @see TcpOptions#NO_DELAY
	 */
	public final void send( byte[] buf, int off, int len )
		throws Exception
	{
		try
		{
			OutputStream os = checkOutputStream();
			os.write( buf, off, len );
			if (options.autoFlush)
				os.flush();
		}
		catch ( IOException e )
		{
			close( true );
			throw e;
		}
		catch ( RuntimeException e )
		{
			close( true );
			throw e;
		}
	}

	/**
	 * Pushes any buffered data to the wire.
	 * @throws IOException
	 */
	public final void flush() throws IOException
	{
		try
		{
			checkOutputStream().flush();
		}
		catch ( IOException e )
		{
			close( true );
			throw e;
		}
		catch ( RuntimeException e )
		{
			close( true );
			throw e;
		}
	}

	/**
	 * @return the output stream if it is still good.
	 * @throws IOException if the output stream is not good.
	 */
	protected final OutputStream checkOutputStream() throws IOException
	{
		OutputStream os = outputStream;
		if (os == null)
			throw new IOException( "closed" );
		return os;
	}

	/**
	 * Reports some data to the session.
	 * @param buf
	 * @throws Exception
	 */
	protected final void fireData( FlexBuffer buf ) throws Exception
	{
		session.sessionData( null, buf );
	}

	/**
	 * This input stream for this connection.
	 */
	protected InputStream inputStream;

	/**
	 * The output stream for this connection.
	 */
	protected OutputStream outputStream;

	@Override
	public SocketAddress localAddress() throws IOException
	{
		return checkSocket().getLocalSocketAddress();
	}

	@Override
	public final SocketAddress remoteAddress() throws IOException
	{
		return checkSocket().getRemoteSocketAddress();
	}

	public final void transportData( Who recipient, FlexBuffer buf ) throws Exception
	{
		send( buf.getBuf(), buf.index(), buf.avail() );
	}

	@Override
	protected final void setupSocket() throws Exception
	{
		Socket s = checkSocket();
		
		s.setKeepAlive( options.keepAlive );
		s.setSoLinger( options.lingerTime >= 0, options.lingerTime >= 0 ? options.lingerTime : 0 );
		s.setTcpNoDelay( options.noDelay );
		s.setTrafficClass( options.trafficClass );
		
		inputStream = s.getInputStream();
		
		outputStream = s.getOutputStream();
		if (options.bufferSize > 0)
			outputStream = new BufferedOutputStream( outputStream, options.bufferSize );
	}

	/**
	 * Shuts down the input channel. This side will get an eof reading the
	 * input. The other side will not be able to tell.
	 * @throws IOException
	 */
	public void shutdownInput() throws IOException
	{
		checkSocket().shutdownInput();
	}

	/**
	 * Shuts down the output channel. This side will get an error trying to
	 * send. The other side will get an eof reading the input.
	 * @throws IOException
	 */
	public void shutdownOutput() throws IOException
	{
		checkSocket().shutdownOutput();
	}
	
	/**
	 * @return a new socket for this connection.
	 * @throws Exception
	 */
	abstract protected Socket newSocket() throws Exception;
	
	/**
	 * @return true if this is a listener initiated connection.
	 */
	abstract protected boolean isServer();
	
	@Override
	public Object transportQuery( Object query ) throws Exception
	{
		if (query == IS_SERVER)
			return isServer();
		
		return super.transportQuery( query );
	}

	@Override
	protected synchronized boolean openSocket( boolean reconnect )
		throws InterruptedException
	{
		// if a one time connection from a server socket listener, just
		// return the existing socket.
		if (!reconnect && socket != null)
			return true;
		
		// if a one time connection from a server socket listener, and
		// this is a reconnect, then bail.
		if (reconnect && isServer())
			return false;
		
		// if a reconnect but no retries allowed, then bail.
		if (reconnect && options.reconnectDelay == 0)
			return false;
		
		// ok, we don't have an existing socket, and this is either the first
		// connection attempt or a reconnect with delay > 0.
		
		boolean first = true;
		
		while (isStarted())
		{
			// if reconnect is false and first is true, this is our
			// very first attempt to connect. otherwise, we are trying
			// to reconnect a broken link or establish a link where we
			// have already failed at least once.
			
			if (reconnect || !first)
			{
				if (options.reconnectDelay == 0)
					return false;
				
				wait( options.reconnectDelay );
				
				if (!isStarted())
					break;
			}
			
			// try to open a socket.
			
			try
			{
				socket = newSocket();
				return true;
			}
			catch ( Exception e )
			{
				if (first)
				{
					first = false;
					fireException( "open", e );
				}
			}
		}
		
		return false;
	}
}
