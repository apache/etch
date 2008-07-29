/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.util.core.io;

import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;

import etch.util.FlexBuffer;
import etch.util.Resources;
import etch.util.URL;
import etch.util.core.Who;

/**
 * Constants and methods related to tcp transport.
 */
abstract public class TcpTransport extends Connection<SessionData>
	implements TransportData
{
	/**
	 * Term on the uri which specifies the auto flush flag. The term string is
	 * "TcpTransport.autoFlush". The value is "true" or "false". The default
	 * is "false".
	 */
	public final static String AUTO_FLUSH = "TcpTransport.autoFlush";

	/**
	 * Term on the uri which specifies the buffer size in bytes. The term
	 * string is "TcpTransport.bufferSize". The value is an integer between
	 * 0 and 65536. The default is 0.
	 */
	public final static String BUFFER_SIZE = "TcpTransport.bufferSize";

	/**
	 * Term on the uri which specifies the keep alive flag. The term string is
	 * "TcpTransport.keepAlive". The value is "true" or "false". The default is
	 * "false".
	 */
	public final static String KEEP_ALIVE = "TcpTransport.keepAlive";

	/**
	 * Term on the uri which specifies the linger time in seconds. The term
	 * string is "TcpTransport.lingerTime". The value is an integer between 0
	 * and 240. The default is 30.
	 */
	public final static String LINGER_TIME = "TcpTransport.lingerTime";

	/**
	 * Term on the uri which specifies the no delay flag. The term string is
	 * "TcpTransport.noDelay". The value is "true" or "false". The default is
	 * "true".
	 */
	public final static String NO_DELAY = "TcpTransport.noDelay";
	
	/**
	 * Term on the uri which specifies the reconnect delay in milliseconds. The
	 * term string is "TcpTransport.reconnectDelay". The value is an integer >=
	 * 0. The default is 0.
	 */
	public final static String RECONNECT_DELAY = "TcpTransport.reconnectDelay";

	/**
	 * Term on the uri which specifies the traffic class. The term string is
	 * "TcpTransport.trafficClass". The value is an integer between 0 and 255.
	 * The default is 0.
	 */
	public final static String TRAFFIC_CLASS = "TcpTransport.trafficClass";

	/**
	 * Constructs the TcpTransport. Pulls common parameters off the uri.
	 *
	 * @param uri
	 * @param resources
	 */
	protected TcpTransport( URL uri, Resources resources )
	{
		setDefaultAutoFlush( uri.getBooleanTerm( AUTO_FLUSH, false ) );
		setDefaultBufferSize( uri.getIntegerTerm( BUFFER_SIZE, 0 ) );
		setDefaultKeepAlive( uri.getBooleanTerm( KEEP_ALIVE, false ) );
		setDefaultLingerTime( uri.getIntegerTerm( LINGER_TIME, 30 ) );
		setDefaultNoDelay( uri.getBooleanTerm( NO_DELAY, true ) );
		setDefaultReconnectDelay( uri.getIntegerTerm( RECONNECT_DELAY, 0 ) );
		setDefaultTrafficClass( uri.getIntegerTerm( TRAFFIC_CLASS, 0 ) );
	}

	private void setDefaultAutoFlush( boolean autoFlush )
	{
		this.autoFlush = autoFlush;
	}
	
	/** The auto flush setting for this connection. If true, each call to send
	 * must automatically call flush. */
	protected boolean autoFlush;
	
	private void setDefaultBufferSize( int bufferSize )
	{
		if (bufferSize < 0 || bufferSize > 65536)
			throw new IllegalArgumentException(
				"bufferSize < 0 || bufferSize > 65536" );
		this.bufferSize = bufferSize;
	}
	
	/** The output buffer size to use for this connection. Bytes, 0 means
	 * unbuffered output. If using buffered output, you'll want to disable
	 * auto flush and call flush manually only when needed. */
	protected int bufferSize;
	
	private void setDefaultKeepAlive( boolean keepAlive )
	{
		this.keepAlive = keepAlive;
	}

	/** The tcp keep alive setting for this connection. */
	protected boolean keepAlive;
	
	private void setDefaultLingerTime( int lingerTime )
	{
		if (lingerTime < -1 || lingerTime > 240)
			throw new IllegalArgumentException(
				"lingerTime < -1 || lingerTime > 240" );
		this.lingerTime = lingerTime;
	}

	/** The tcp linger time setting for this connection. Time in seconds, -1
	 * means disable. */
	protected int lingerTime;
	
	private void setDefaultNoDelay( boolean noDelay )
	{
		this.noDelay = noDelay;
	}

	/** The tcp no delay setting for this connection. True disables nagle's
	 * algorithm and causes all sends to be made asap. */
	protected boolean noDelay;
	
	private void setDefaultReconnectDelay( int reconnectDelay )
	{
		if (reconnectDelay < 0)
			throw new IllegalArgumentException(
				"reconnectDelay < 0" );
		this.reconnectDelay = reconnectDelay;
	}
	
	/** The reconnect delay for this connection. Time in milliseconds, 0 means
	 * do not reconnect. */
	protected int reconnectDelay;
	
	private void setDefaultTrafficClass( int trafficClass )
	{
		if (trafficClass < 0 || trafficClass > 255)
			throw new IllegalArgumentException(
				"trafficClass < 0 || trafficClass > 255" );
		this.trafficClass = trafficClass;
	}

	/** The traffic class for this connection. 0-255, 0 means normal handling.
	 * Also called type of service or dscp. */
	protected int trafficClass;
	
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
	 * @see #AUTO_FLUSH
	 * @see #BUFFER_SIZE
	 * @see #NO_DELAY
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
	 * @see #AUTO_FLUSH
	 * @see #BUFFER_SIZE
	 * @see #NO_DELAY
	 */
	public final void send( byte[] buf, int off, int len )
		throws Exception
	{
		try
		{
			OutputStream os = checkOutputStream();
			os.write( buf, off, len );
			if (autoFlush)
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
		
		s.setKeepAlive( keepAlive );
		s.setSoLinger( lingerTime >= 0, lingerTime >= 0 ? lingerTime : 0 );
		s.setTcpNoDelay( noDelay );
		s.setTrafficClass( trafficClass );
		
		inputStream = s.getInputStream();
		
		outputStream = s.getOutputStream();
		if (bufferSize > 0)
			outputStream = new BufferedOutputStream( outputStream, bufferSize );
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
		if (reconnect && reconnectDelay == 0)
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
				if (reconnectDelay == 0)
					return false;
				
				wait( reconnectDelay );
				
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
