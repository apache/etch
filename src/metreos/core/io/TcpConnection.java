/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems. All rights reserved.
 */

package metreos.core.io;

import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;

import metreos.core.Who;
import metreos.util.FlexBuffer;
import metreos.util.URL;
import etch.bindings.java.transport.Resources;

/**
 * Implementation of connection which handles a tcp connection.
 */
public class TcpConnection extends Connection<DataHandler>
	implements DataSource
{
	/**
	 * Constructs the Connection using an existing socket. No attempt
	 * is made to reconnect if the connection goes down.
	 *
	 * @param handler target to receive data events.
	 * @param socket existing connected socket (e.g., from Listener).
	 */
	public TcpConnection( DataHandler handler, Socket socket )
	{
		super( handler );
		
		if (socket == null)
			throw new IllegalArgumentException( "socket == null" );

		this.socket = socket;
		this.host = null;
		this.port = 0;
		this.delay = 0;
	}
	
	/**
	 * Constructs the Connection. If delay is greater than 0, an attempt
	 * will be made to reconnect a broken connection, or retry a
	 * failed connection attempt.
	 *
	 * @param handler target to receive data events.
	 * @param host address to connect to. Host cannot be null.
	 * @param port port to connect to. Port must be > 0 and < 65536.
	 * @param delay how long in milliseconds to wait before retrying a
	 * failure. Delay must be >= 0. Delay of 0 means do not retry.
	 */
	public TcpConnection( DataHandler handler, String host, int port,
		int delay )
	{
		super( handler );
		
		if (host == null)
			throw new IllegalArgumentException( "host == null" );
		
		if (port <= 0 || port >= 65536)
			throw new IllegalArgumentException( "port <= 0 || port >= 65536" );
		
		if (delay < 0)
			throw new IllegalArgumentException( "delay < 0" );
		
		this.socket = null;
		this.host = host;
		this.port = port;
		this.delay = delay;
	}
	
	/**
	 * Term on the uri which specifies the reconnect delay.
	 */
	public final static String RECONNECT_DELAY = "TcpConnection.reconnect_delay";

	/**
	 * Term on the uri which specifies the default auto flush flag.
	 * @see #setDefaultAutoFlush(boolean)
	 */
	private static final String AUTO_FLUSH = "TcpConnection.autoFlush";

	/**
	 * Term on the uri which specifies the default keep alive flag.
	 * @see #setDefaultKeepAlive(boolean)
	 */
	private static final String KEEP_ALIVE = "TcpConnection.keepAlive";

	/**
	 * Term on the uri which specifies the default linger time.
	 * @see #setDefaultSoLinger(boolean, int)
	 */
	private static final String LINGER_TIME = "TcpConnection.lingerTime";

	/**
	 * Term on the uri which specifies the default no delay flag.
	 * @see #setDefaultNoDelay(boolean)
	 */
	private static final String NO_DELAY = "TcpConnection.noDelay";

	/**
	 * Term on the uri which specifies the default traffic class.
	 * @see #setDefaultTrafficClass(int)
	 */
	private static final String TRAFFIC_CLASS = "TcpConnection.trafficClass";

	/**
	 * Term on the uri which specifies the default buffer size parameter.
	 * @see #setDefaultBufferSize(int)
	 */
	private static final String BUFFER_SIZE = "TcpConnection.bufferSize";
	
	/**
	 * Constructs the Connection from a uri.
	 * @param uri
	 * @param resources 
	 * @param socket
	 */
	public TcpConnection( URL uri, Resources resources, Socket socket )
	{
		super( null );
		
		if (socket == null)
		{
			String host = uri.getHost();
			if (host == null)
				throw new IllegalArgumentException( "host == null" );
			
			Integer port = uri.getPort();
			if (port == null)
				throw new IllegalArgumentException( "port == null" );
			
			if (port <= 0 || port >= 65536)
				throw new IllegalArgumentException( "port <= 0 || port >= 65536" );
			
			int reconnectDelay = uri.getIntegerTerm( RECONNECT_DELAY, 0 );
			if (reconnectDelay < 0)
				throw new IllegalArgumentException( "delay < 0" );
			
			this.socket = null;
			this.host = host;
			this.port = port;
			this.delay = reconnectDelay;
		}
		else
		{
			this.socket = socket;
			this.host = null;
			this.port = 0;
			this.delay = 0;
		}
		
		Boolean defAutoFlush = uri.getBooleanTerm( AUTO_FLUSH );
		if (defAutoFlush != null)
			setDefaultAutoFlush( defAutoFlush );
		
		Boolean defKeepAlive = uri.getBooleanTerm( KEEP_ALIVE );
		if (defKeepAlive != null)
			setDefaultKeepAlive( defKeepAlive );
		
		Integer defLingerTime = uri.getIntegerTerm( LINGER_TIME );
		if (defLingerTime != null)
			setDefaultLingerTime( defLingerTime );
		
		Boolean defNoDelay = uri.getBooleanTerm( NO_DELAY );
		if (defNoDelay != null)
			setDefaultNoDelay( defNoDelay );
		
		Integer defTrafficClass = uri.getIntegerTerm( TRAFFIC_CLASS );
		if (defTrafficClass != null)
			setDefaultTrafficClass( defTrafficClass );
		
		Integer defBufferSize = uri.getIntegerTerm( BUFFER_SIZE );
		if (defBufferSize != null)
			setDefaultBufferSize( defBufferSize );
	}

	private Socket socket;
	
	private final String host;
	
	private final int port;
	
	private final int delay;
	
	@Override
	public String toString()
	{
		if (socket != null)
			return String.format( "Tcp(up, %s, %d)", socket.getInetAddress(), socket.getPort() );
		
		return String.format( "Tcp(down, %s, %d)", host, port );
	}
	
	/**
	 * Sets the default value of the keepAlive flag. Note that this is a
	 * tcp keep alive, which is not universally supported and claimed by
	 * some to slow down connection performance. Also, tcp keep alive only
	 * sends a protocol level packet every 2 hrs if there is no other traffic,
	 * which means you won't really get timely notice if the peer or network
	 * goes down. It is probably better to implement an application level
	 * keep alive.
	 * @param keepAlive true enables tcp keep alive, requesting the periodic
	 * exchange of protocol packets to keep the tcp connection alive. The
	 * default is false.
	 */
	public void setDefaultKeepAlive( boolean keepAlive )
	{
		this.keepAlive = keepAlive;
	}

	private boolean keepAlive = false;
	
	/**
	 * Sets the default value of the lingerTime parameter.
	 * @param lingerTime the linger time in seconds. A negative value requests
	 * platform defined handling of close. A zero value causes an immediate
	 * discarding of any buffered data and a reset of the connection upon
	 * close. A positive value causes close to delay for up to that many
	 * seconds for tcp to transmit and receive acknowledgement of any pending
	 * data. The default is 30 seconds.
	 * @see #shutdownInput()
	 * @see #shutdownOutput()
	 */
	public void setDefaultLingerTime( int lingerTime )
	{
		this.lingerTime = lingerTime;
	}

	private int lingerTime = 30;
	
	/**
	 * Sets the default value for the tcp noDelay parameter. Normally
	 * true, enabling noDelay causes data queued for transmission to
	 * be sent asap. When disabled, data may be delayed a bit to allow
	 * it to be combined with other data moving through the channel.
	 * Also known as "Nagle's Algorithm". When sending "packetized
	 * data" on a stream, such as ipc/rpc, noDelay should be enabled.
	 * When sending large chunks of stream data, such as http requests
	 * and responses, noDelay should be disabled. In any event, calling
	 * flush usually forces any queued data onto the wire anyway.
	 * 
	 * @param tcpNoDelay true enables noDelay. The default is true.
	 * 
	 * @see #flush()
	 */
	public void setDefaultNoDelay( boolean tcpNoDelay )
	{
		this.tcpNoDelay = tcpNoDelay;
	}

	private boolean tcpNoDelay = true;
	
	/**
	 * Sets the default value for the traffic class (or type of service
	 * or dscp). For example, call signalling traffic is classified as
	 * DSCP CS3 (011000) with resulting traffic class value of 01100000,
	 * 0x60, or 96. For more on DSCP see RFC 2474 (supercedes TOS defined
	 * in RFC 791)
	 * @param trafficClass traffic class or type of service or dscp. The
	 * default is 0, which implies normal handling.
	 */
	public void setDefaultTrafficClass( int trafficClass )
	{
		this.trafficClass = trafficClass;
	}
	
	private int trafficClass = 0;
	
	/**
	 * Sets the default value for the buffer size parameter. If using buffered
	 * sends, you'll probably want to disable auto flush and flush manually
	 * only when needed.
	 * @param bufferSize positive if calls to send should be buffered, zero if
	 * calls to send should go to the socket directly. The default is 0.
	 * @see #flush()
	 * @see #setDefaultAutoFlush(boolean)
	 */
	public void setDefaultBufferSize( int bufferSize )
	{
		this.bufferSize = bufferSize;
	}
	
	private int bufferSize = 0;
	
	/**
	 * Sets the default value of the autoFlush flag.
	 * @param autoFlush true if each call to send should be automatically
	 * flushed. The default is false.
	 * @see #send(byte[])
	 * @see #send(byte[], int, int)
	 * @see #flush()
	 */
	public void setDefaultAutoFlush( boolean autoFlush )
	{
		this.autoFlush = autoFlush;
	}
	
	private boolean autoFlush = false;
	
	@Override
	protected void stop0() throws Exception
	{
		close( false );
		super.stop0();
	}
	
	private Socket checkSocket() throws SocketException
	{
		Socket s = socket;
		
		if (s == null)
			throw new SocketException( "socket closed" );
		
		return s;
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
		if (reconnect && host == null)
			return false;
		
		// if a reconnect but no retries allowed, then bail.
		if (reconnect && delay <= 0)
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
				if (delay == 0)
					return false;
				
				wait( delay );
				
				if (!isStarted())
					break;
			}
			
			// try to open a socket.
			
			try
			{
				socket = new Socket( host, port );
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

	@Override
	protected void setupSocket() throws Exception
	{
		Socket s = checkSocket();
		
		s.setKeepAlive( keepAlive );
		s.setSoLinger( lingerTime >= 0, lingerTime >= 0 ? lingerTime : 0 );
		s.setTcpNoDelay( tcpNoDelay );
		s.setTrafficClass( trafficClass );
		
		outputStream = s.getOutputStream();
		if (bufferSize > 0)
			outputStream = new BufferedOutputStream( outputStream, bufferSize );
	}
	
	@Override
	protected void readSocket() throws Exception
	{
		final Socket s = checkSocket();
		final InputStream is = s.getInputStream();
		// TODO allow buffer size to be specified.
		final FlexBuffer buf = new FlexBuffer( new byte[8192] );
		
		try
		{
			while (isStarted())
			{
//				System.out.println( "reading" );
				int n = is.read( buf.getBuf() );
//				System.out.println( "read "+n );
				
				if (n <= 0)
					break;
				
				buf.setLength( n );
				buf.setIndex( 0 );
				fireData( buf );
			}
		}
		catch ( SocketException e )
		{
			String msg = e.getMessage();
			if (msg == null)
				throw e;
			
			msg = msg.toLowerCase();
			
			if (msg.indexOf( "socket closed" ) >= 0)
				return;
			
			if (msg.indexOf( "recv failed" ) >= 0)
				return;
			
			throw e;
		}
	}
	
	@Override
	public void close( boolean reset ) throws IOException
	{
		Socket s = socket;
		if (s != null)
		{
			try
			{
				try
				{
					if (reset)
					{
						s.setSoLinger( true, 0 );
					}
					else
					{
						flush();
						shutdownOutput();
					}
				}
				finally
				{
					s.close();
				}
			}
			catch ( SocketException e )
			{
				// ignore.
			}
			finally
			{
				outputStream = null;
				socket = null;
			}
		}
	}

	/////////////////////////
	// DATA SOURCE METHODS //
	/////////////////////////

	public void data( Who recipient, FlexBuffer buf ) throws Exception
	{
		send( buf.getBuf(), buf.index(), buf.avail() );
	}

	/**
	 * Sends some data to the remote end. The output data is buffered
	 * until the buffer is full or the buffer is flushed.
	 * @param buf the bytes to be sent.
	 * @throws Exception if there is a problem transmitting the
	 * data. Such a problem causes the current connection to be
	 * reset.
	 * @see #flush()
	 * @see #setDefaultAutoFlush(boolean)
	 */
	public void send( byte[] buf ) throws Exception
	{
		send( buf, 0, buf.length );
	}
	
	/**
	 * Sends some data to the remote end. The output data is buffered
	 * until the buffer is full or the buffer is flushed.
	 * @param buf the bytes to be sent.
	 * @param off the offset into buf of the first byte to send.
	 * @param len the number of bytes to send.
	 * @throws IOException if there is a problem transmitting the
	 * data. Such a problem causes the current connection to be
	 * reset.
	 * @see #flush()
	 * @see #setDefaultAutoFlush(boolean)
	 */
	public void send( byte[] buf, int off, int len ) throws IOException
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
	public void flush() throws IOException
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
	
	private OutputStream checkOutputStream() throws IOException
	{
		OutputStream os = outputStream;
		if (os == null)
			throw new IOException( "closed" );
		return os;
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
	
	private OutputStream outputStream;
	
	private void fireData( FlexBuffer buf ) throws Exception
	{
		handler.data( null, buf );
	}

	public SocketAddress localAddress() throws IOException
	{
		return checkSocket().getLocalSocketAddress();
	}

	public SocketAddress remoteAddress() throws IOException
	{
		return checkSocket().getRemoteSocketAddress();
	}

//	public Object oobQuerySource( Object query ) throws Exception
//	{
//		return super.oobQuerySource( query );
//	}

//	public void transportControl( Object control, Object value ) throws Exception
//	{
//		super.transportControl( control, value );
//	}

//	public void oobNotifySource( Object event ) throws Exception
//	{
//		super.oobNotifySource( event );
//	}
}
