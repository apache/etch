package etch.util.core.io;

import java.io.FileInputStream;
import java.io.IOException;
import java.net.BindException;
import java.net.InetAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.security.KeyStore;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLServerSocket;
import javax.net.ssl.SSLServerSocketFactory;
import javax.net.ssl.SSLSocket;

import etch.util.Resources;
import etch.util.URL;

public class TlsListener extends Connection<SessionListener>
implements Transport<SessionListener>
{
	
	/**
	 * Query term for URI to specify backlog value to ServerSocket. The value
	 * is "TcpListener.backlog". The backlog must be >= 0 and somewhat
	 * reasonable. The value 0 specifies the system default, the same as if you
	 * leave the term off altogether.
	 * 
	 * Example: tcp://host:port?TcpListener.backlog=250
	 * 
	 * @see #TcpListener(String, Resources)
	 */
	public final static String BACKLOG = "TlsListener.backlog";
	
	/**
	 * Constructs a Listener, initialized from the URI. Listens on the host
	 * and port specified in the URI. To listen on all interfaces, specify
	 * host as ALL_INTFS ("0.0.0.0"). If port is specified or defaulted to 0,
	 * an unused port will be selected.
	 * @param uri
	 * @param resources 
	 */
	public TlsListener( String uri, Resources resources )
	{
		this( new URL( uri ), resources );
	}
	
	/**
	 * Constructs a Listener, initialized from the URI. Listens on the host
	 * and port specified in the URI. To listen on all interfaces, specify
	 * host as ALL_INTFS ("0.0.0.0"). If port is specified or defaulted to 0,
	 * an unused port will be selected.
	 * @param uri
	 * @see URL#getHost()
	 * @see URL#getPort()
	 * @see URL#getIntegerTerm(String, int)
	 * @see #BACKLOG
	 * @see #ALL_INTFS
	 * @see #localAddress()
	 */
	private TlsListener( URL uri, Resources resources )
	{
		this( translateHost( uri.getHost() ), uri.getPort(),
			uri.getIntegerTerm( BACKLOG, 0 ) );
	}

	/**
	 * Constructs the Listener.
	 * @param host address to listen to. Null means any local address.
	 * @param port port to listen on. Port must be >= 0. Port of 0
	 * means allocate an available port.
	 * @param backlog max number of pending connection requests.
	 */
	private TlsListener( String host, int port, int backlog )
	{
		if (port < 0 || port > 65535)
			throw new IllegalArgumentException( "port < 0 || port > 65535" );
		
		if (backlog < 0)
			throw new IllegalArgumentException( "backlog < 0" );
		
		this.host = host;
		this.port = port;
		this.backlog = backlog;
	}
	
	private final String host;
	
	private final int port;
	
	private final int backlog;
	
	private String KEYSTORE = null;
	private char[] KEYSTOREPW = null;
	private char[] KEYPAIRPWD = null;
	
	@Override
	protected void stop0() throws Exception
	{
		close( true );
		super.stop0();
	}
	
	private SSLServerSocket checkSocket() throws IOException
	{
		SSLServerSocket ss = sslServerSocket;
		if (ss == null)
			throw new SocketException( "socket closed" );
		return ss;
	}

	private SSLServerSocket sslServerSocket;
	
	@Override
	public String toString()
	{
		if (sslServerSocket == null)
			return String.format( "Listener(down, %s, %d)", host, port );
		
		return String.format( "Listener(up, %s, %d)",
			sslServerSocket.getInetAddress(), sslServerSocket.getLocalPort() );
	}

	@Override
	protected synchronized boolean openSocket( boolean reconnect ) throws Exception
	{
		boolean first = true;
		while (isStarted())
		{
			if (reconnect || !first)
				return false;
			
			try
			{
				InetAddress h = host != null ? InetAddress.getByName( host ) : null;
				try
				{
	//				serverSocket = new ServerSocket( port, backlog, h );
					initializeSSLVariables();
					KeyStore keystore = KeyStore.getInstance("JKS");
					keystore.load(new FileInputStream(KEYSTORE), KEYSTOREPW);
					KeyManagerFactory kmf = KeyManagerFactory.getInstance("SunX509");  
					kmf.init(keystore, KEYPAIRPWD);
					SSLContext sslc = SSLContext.getInstance("SSLv3");
					sslc.init(kmf.getKeyManagers(), null, null);		         
					SSLServerSocketFactory sslServerSocketFactory = sslc.getServerSocketFactory(); 
					
			/*		SSLServerSocketFactory sslServerSocketFactory =
		                   (SSLServerSocketFactory) SSLServerSocketFactory.getDefault();   */
		            sslServerSocket =
		                    (SSLServerSocket) sslServerSocketFactory.createServerSocket( port, backlog, h);
				}
				catch ( BindException e )
				{
					throw new BindException( "Address already in use: "+h+":"+port );
				}
				catch (IOException ex) 
				{
					IOException exc = new IOException(" Problem in setting up SSL Connection. In order to use SSL " +
						 " please specify valid keystore location and password using VM arguments." );
					exc.setStackTrace( ex.getStackTrace() );				
					fireException("open",exc);
					return false;
				}
				
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
		// nothing to do.
	}

	@Override
	protected void readSocket() throws Exception
	{
		SSLServerSocket ss = checkSocket();
		while (isStarted())
		{
			SSLSocket s = (SSLSocket)ss.accept();
			try
			{
				if (session == null)
					throw new NullPointerException( "session == null" );
				session.sessionAccepted( s );
			}
			catch ( Exception e )
			{
				s.close();
				fireException( "sessionAccepted", e );
			}
		}
	}

	@Override
	public void close( boolean reset ) throws Exception
	{
		SSLServerSocket ss = sslServerSocket;
		if (ss != null)
		{
			sslServerSocket = null;
			ss.close();
		}
	}

	public SocketAddress localAddress() throws IOException
	{
		return checkSocket().getLocalSocketAddress();
	}

	public SocketAddress remoteAddress()
	{
		// ignore.
		return null;
	}
	
	private void initializeSSLVariables () throws Exception
	{
		 String keyStore = System.getProperty("javax.net.ssl.keyStore");
		  if(keyStore == null) {
			  System.out.println("javax.net.ssl.keyStore is not defined");
			  throw new Exception(" keyStore variable is not defined. Please define keystore location using" +
			  		" VM Arguments");
		  }
		  else 
		  {
			  KEYSTORE = keyStore;
			  System.out.println("javax.net.ssl.keyStore = " + KEYSTORE);
		  }
		  
		  String keypwd = System.getProperty("javax.net.ssl.keyStorePassword");
		  if(keypwd == null)
		  {
			  System.out.println("javax.net.ssl.keyStorePassword is not defined");
			  throw new Exception(" KeyStore Password is not define. Please define keystore password using" +
		  		" VM Arguments");
		  }
		  else 
		  {
			  KEYSTOREPW = keypwd.toCharArray();
		  }
		  		  
		  String keypairpwd = System.getProperty("javax.net.ssl.keyPairPassword");
		  if(keypairpwd == null) {
		   System.out.println("javax.net.ssl.keyPairPassword is not defined");
		   KEYPAIRPWD = KEYSTOREPW;
		  }
		  else {
			  KEYPAIRPWD = keypairpwd.toCharArray();
		  }
	}

}
