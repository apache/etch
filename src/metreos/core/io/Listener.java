/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package metreos.core.io;

import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;

import metreos.util.URL;

/**
 * Implementation of a connection which handles a socket listener.
 */
public class Listener extends Connection<ListenerHandler>
{
	/**
	 * Constructs the Listener.
	 * @param handler target to receive listener events.
	 * @param backlog max number of pending connection requests.
	 * @param host address to listen to. Null means any local address.
	 * @param port port to listen on. Port must be >= 0. Port of 0
	 * means allocate an available port.
	 * @param delay how long in milliseconds to wait before retrying a
	 * failure. Delay must be >= 0. Delay of 0 means do not retry.
	 */
	public Listener( ListenerHandler handler, int backlog, String host,
		int port, int delay )
	{
		super( handler );
		
		if (backlog < 0)
			throw new IllegalArgumentException( "backlog < 0" );
		
		if (port < 0 || port > 65535)
			throw new IllegalArgumentException( "port < 0 || port > 65535" );
		
		if (delay < 0)
			throw new IllegalArgumentException( "delay < 0" );
		
		this.backlog = backlog;
		this.host = host;
		this.port = port;
		this.delay = delay;
	}
	
	/**
	 * Query term from URI to specify backlog value to ServerSocket. The
	 * value is "TcpListener.backlog".
	 * @see #Listener(URL)
	 */
	public final static String BACKLOG = "TcpListener.backlog";
	
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
	public Listener( URL uri )
	{
		this( null, uri.getIntegerTerm( BACKLOG, 0 ),
			translateHost( uri.getHost() ), uri.getPort(), 0 );
	}

	private final int backlog;
	
	private final String host;
	
	private final int port;
	
	private final int delay;
	
	@Override
	public void setHandler( ListenerHandler handler )
	{
		super.setHandler( handler );
		handler.setSource( this );
	}
	
	@Override
	protected void stop0() throws Exception
	{
		close( true );
		super.stop0();
	}
	
	private ServerSocket checkSocket() throws IOException
	{
		ServerSocket ss = serverSocket;
		if (ss == null)
			throw new SocketException( "socket closed" );
		return ss;
	}

	private ServerSocket serverSocket;
	
	@Override
	public String toString()
	{
		if (serverSocket == null)
			return String.format( "Listener(down, %s, %d)", host, port );
		
		return String.format( "Listener(up, %s, %d)",
			serverSocket.getInetAddress(), serverSocket.getLocalPort() );
	}

	@Override
	protected synchronized boolean openSocket( boolean reconnect ) throws Exception
	{
		boolean first = true;
		while (isStarted())
		{
			if (reconnect || !first)
			{
				if (delay == 0)
					return false;
				
				wait( delay );
				
				if (!isStarted())
					break;
			}
			
			try
			{
				serverSocket = new ServerSocket( port, backlog,
					host != null ? InetAddress.getByName( host ) : null );
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
		ServerSocket ss = checkSocket();
		while (isStarted())
		{
			Socket s = ss.accept();
			try
			{
				handler.accepted( s );
			}
			catch ( Exception e )
			{
				s.close();
				fireException( "accepted", e );
			}
		}
	}

	@Override
	public void close( boolean reset ) throws Exception
	{
		ServerSocket ss = serverSocket;
		if (ss != null)
		{
			serverSocket = null;
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
}
