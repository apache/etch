package etch.util.core.io;

import java.io.BufferedOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;

import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLHandshakeException;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.X509TrustManager;

import etch.util.FlexBuffer;
import etch.util.Resources;
import etch.util.URL;
import etch.util.core.Who;

/**
 * A Connection which manages a tls encoded tcp data stream.
 */
public class TlsConnection extends Connection<SessionData>
implements TransportData
{
	/**
	 * Term on the uri which specifies the reconnect delay. The term string
	 * is "TcpConnection.reconnect_delay".
	 */
	public final static String RECONNECT_DELAY = "TlsConnection.reconnect_delay";

	/**
	 * Term on the uri which specifies the default auto flush flag. The
	 * term string is "TcpConnection.autoFlush".
	 * @see #setDefaultAutoFlush(boolean)
	 */
	public static final String AUTO_FLUSH = "TlsConnection.autoFlush";

	/**
	 * Term on the uri which specifies the default keep alive flag. The term
	 * string is "TcpConnection.keepAlive".
	 * @see #setDefaultKeepAlive(boolean)
	 */
	public static final String KEEP_ALIVE = "TlsConnection.keepAlive";

	/**
	 * Term on the uri which specifies the default linger time. The term string
	 * is "TcpConnection.lingerTime".
	 * @see #setDefaultLingerTime(int)
	 */
	public static final String LINGER_TIME = "TlsConnection.lingerTime";

	/**
	 * Term on the uri which specifies the default no delay flag. The term
	 * string is "TcpConnection.noDelay".
	 * @see #setDefaultNoDelay(boolean)
	 */
	public static final String NO_DELAY = "TlsConnection.noDelay";

	/**
	 * Term on the uri which specifies the default traffic class. The term
	 * string is "TcpConnection.trafficClass".
	 * @see #setDefaultTrafficClass(int)
	 */
	public static final String TRAFFIC_CLASS = "TlsConnection.trafficClass";

	/**
	 * Term on the uri which specifies the default buffer size parameter. The
	 * term string is "TcpConnection.bufferSize".
	 * @see #setDefaultBufferSize(int)
	 */
	public static final String BUFFER_SIZE = "TlsConnection.bufferSize";
	
	
	/**
	 * Term on uri which specifies whether server certificate should be authenticated. 
	 */
   
    private static final String CERT_AUTH_REQUIRED = "TlsConnection.authReqd";

    
    private boolean isAuthReqd = true;
	
	/**
	 * Constructs the Connection from a uri.
	 * @param socket
	 * @param uri
	 * @param resources 
	 */
	public TlsConnection( Socket socket, String uri, Resources resources )
	{
		URL u = new URL( uri );
		
		
		if (socket == null)
		{
			String host = u.getHost();
			if (host == null)
				throw new IllegalArgumentException( "host == null" );
			
			Integer port = u.getPort();
			if (port == null)
				throw new IllegalArgumentException( "port == null" );
			
			if (port <= 0 || port > 65535)
				throw new IllegalArgumentException( "port <= 0 || port > 65535" );
			
			int reconnectDelay = u.getIntegerTerm( RECONNECT_DELAY, 0 );
			if (reconnectDelay < 0)
				throw new IllegalArgumentException( "delay < 0" );
			
			isAuthReqd = (boolean) u.getBooleanTerm(CERT_AUTH_REQUIRED,true);
			
			this.sslSocket = null;
			this.host = host;
			this.port = port;
			this.delay = reconnectDelay;
		}
		else
		{
			this.sslSocket = (SSLSocket)socket;
			this.host = null;
			this.port = 0;
			this.delay = 0;
		}
		
		setDefaultAutoFlush( u.getBooleanTerm( AUTO_FLUSH, false ) );
		setDefaultBufferSize( u.getIntegerTerm( BUFFER_SIZE, 0 ) );
		setDefaultKeepAlive( u.getBooleanTerm( KEEP_ALIVE, false ) );
		setDefaultLingerTime( u.getIntegerTerm( LINGER_TIME, 30 ) );
		setDefaultNoDelay( u.getBooleanTerm( NO_DELAY, true ) );
		setDefaultTrafficClass( u.getIntegerTerm( TRAFFIC_CLASS, 0 ) );
	}

	private SSLSocket sslSocket;
	
	private final String host;
	
	private final int port;
	
	private final int delay;
	
	@Override
	public String toString()
	{
		SSLSocket s = sslSocket;
		
		if (s != null)
			return String.format(
				"Tls(up, %s:%d - %s:%d)",
				s.getLocalAddress(), s.getLocalPort(),
				s.getInetAddress(), s.getPort() );
		
		return String.format( "Tls(down, %s:%d)", host, port );
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
	 * @param lingerTime the linger time in seconds. A value of -1 requests
	 * platform defined handling of close. A zero value causes an immediate
	 * discarding of any buffered data and a reset of the connection upon
	 * close. A positive value causes close to delay for up to that many
	 * seconds for tcp to transmit and receive acknowledgement of any pending
	 * data and close. The default is 30 seconds.
	 * @see #shutdownInput()
	 * @see #shutdownOutput()
	 */
	public void setDefaultLingerTime( int lingerTime )
	{
		if (lingerTime < -1 || lingerTime > 240)
			throw new IllegalArgumentException(
				"lingerTime < -1 || lingerTime > 240" );
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
		if (trafficClass < 0 || trafficClass > 255)
			throw new IllegalArgumentException(
				"trafficClass < 0 || trafficClass > 255" );
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
		if (bufferSize < 0 || bufferSize > 65536)
			throw new IllegalArgumentException(
				"bufferSize < 0 || bufferSize > 65536" );
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
	
	private SSLSocket checkSocket() throws SocketException
	{
		SSLSocket s = sslSocket;
		
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
		if (!reconnect && sslSocket != null)
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
				SSLSocketFactory sslsocketfactory = null;							  
				 if (isAuthReqd)
				 {
					 sslsocketfactory = (SSLSocketFactory) SSLSocketFactory.getDefault();
				 }
				 else 
				 {
					 TrustManager[] trustAllCerts = new TrustManager[]{
					        new X509TrustManager() {
					            public java.security.cert.X509Certificate[] getAcceptedIssuers() {
					                return null;
					            }
					            public void checkClientTrusted(
					                java.security.cert.X509Certificate[] certs, String authType) {
					            }
					            public void checkServerTrusted(
					                java.security.cert.X509Certificate[] certs, String authType) {
					            }
					        }
					    };
					 SSLContext sslc = SSLContext.getInstance("SSLv3");
					 sslc.init(null, trustAllCerts, null);
					 sslsocketfactory = sslc.getSocketFactory(); 
				 }
	            sslSocket = (SSLSocket) sslsocketfactory.createSocket(host, port);
				
				return true;
			}
			
			catch ( Exception e )
			{
//				Log.report( "newSocketFailed",
//					"host", host,
//					"port", port,
//					Log.EXCP, e );

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
		SSLSocket s = checkSocket();
		
		s.setKeepAlive( keepAlive );
		s.setSoLinger( lingerTime >= 0, lingerTime >= 0 ? lingerTime : 0 );
		s.setTcpNoDelay( tcpNoDelay );
		s.setTrafficClass( trafficClass );
		
		inputStream = s.getInputStream();
		
		
		
		outputStream = s.getOutputStream();
		if (bufferSize > 0)
			outputStream = new BufferedOutputStream( outputStream, bufferSize );
	}
	
	@Override
	protected void readSocket() throws Exception
	{
		final InputStream is = inputStream;
		if (is == null)
			throw new IOException( "socket closed" );
		
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
		catch(SSLHandshakeException ex) {
			SSLHandshakeException exc = new SSLHandshakeException(" Problem in setting up SSL Connection. Either disable" +
				 " server authentication by using flag TlsConnection.authReqd=false in uri or provide" +
				" a trustsore location which contains the valid certificate");
	//		exc.setStackTrace( ex.getStackTrace() );
			throw exc;
		}
		catch ( SocketException e )
		{
//			Log.report( "readSocketFailed",
//				"s.local", s.getLocalSocketAddress(),
//				"s.remote", s.getRemoteSocketAddress(),
//				"e", e.toString() );
			
			String msg = e.getMessage();
			if (msg == null)
				throw e;
			
			msg = msg.toLowerCase();
			
			if (msg.indexOf( "socket closed" ) >= 0)
				return;
			
			if (msg.indexOf( "recv failed" ) >= 0)
				return; 
			
			if (msg.indexOf( "connection reset" ) >= 0)
				return; 
			
			throw e;
		}
	}
	
	@Override
	public void close( boolean reset ) throws IOException
	{
//		Log.report( "close" );
		
		SSLSocket s = sslSocket;
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
//					Log.report( "closing" );
					s.close();
//					Log.report( "closed" );
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
				sslSocket = null;
			}
		}
	}

	/////////////////////////
	// DATA SOURCE METHODS //
	/////////////////////////

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
//		checkSocket().
	}
	
	private InputStream inputStream;
	
	private OutputStream outputStream;
	
	private void fireData( FlexBuffer buf ) throws Exception
	{
		session.sessionData( null, buf );
	}

	public SocketAddress localAddress() throws IOException
	{
		return checkSocket().getLocalSocketAddress();
	}

	public SocketAddress remoteAddress() throws IOException
	{
		return checkSocket().getRemoteSocketAddress();
	}

	public void transportData( Who recipient, FlexBuffer buf ) throws Exception
	{
		send( buf.getBuf(), buf.index(), buf.avail() );
	}

}
