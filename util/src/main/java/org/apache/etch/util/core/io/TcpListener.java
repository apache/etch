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

import java.io.IOException;
import java.net.BindException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;

import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;


/**
 * Implementation of a connection which handles a socket listener.
 */
public class TcpListener extends Connection<SessionListener<Socket>>
	implements Transport<SessionListener<Socket>>
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
	public final static String BACKLOG = "TcpListener.backlog";
	
	/**
	 * Constructs a TcpListener, initialized from the URI. Listens on the host
	 * and port specified in the URI. To listen on all interfaces, specify
	 * host as ALL_INTFS ("0.0.0.0"). If port is specified or defaulted to 0,
	 * an unused port will be selected.
	 * @param uri
	 * @param resources
	 */
	public TcpListener( String uri, Resources resources )
	{
		this( new URL( uri ), resources );
	}
	
	/**
	 * Constructs a TcpListener, initialized from the URI. Listens on the host
	 * and port specified in the URI. To listen on all interfaces, specify
	 * host as ALL_INTFS ("0.0.0.0"). If port is specified or defaulted to 0,
	 * an unused port will be selected.
	 * @param uri
	 * @param resources 
	 * @see URL#getHost()
	 * @see URL#getPort()
	 * @see URL#getIntegerTerm(String, int)
	 * @see #BACKLOG
	 * @see #ALL_INTFS
	 * @see #localAddress()
	 */
	public TcpListener( URL uri, Resources resources )
	{
		this( translateHost( uri.getHost() ), uri.getPort(),
			uri.getIntegerTerm( BACKLOG, 0 ) );
	}

	/**
	 * Constructs the TcpListener.
	 * @param host address to listen to. Null means any local address.
	 * @param port port to listen on. Port must be >= 0. Port of 0
	 * means allocate an available port.
	 * @param backlog max number of pending connection requests.
	 */
	private TcpListener( String host, int port, int backlog )
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
		ServerSocket s = serverSocket;
		
		if (s != null)
			return String.format( "TcpListener(up, %s, %d)",
				s.getInetAddress(), s.getLocalPort() );
		
		return String.format( "TcpListener(down, %s, %d)", host, port );
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
					serverSocket = new ServerSocket( port, backlog, h );
					return true;
				}
				catch ( BindException e )
				{
					BindException x = new BindException( "Cannot assign requested address: "+h+":"+port );
					x.initCause( e );
					throw x;
				}
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
		ServerSocket ss = serverSocket;
		if (ss != null)
		{
			serverSocket = null;
			ss.close();
		}
	}

	@Override
	public SocketAddress localAddress() throws IOException
	{
		return checkSocket().getLocalSocketAddress();
	}

	@Override
	public SocketAddress remoteAddress()
	{
		// ignore.
		return null;
	}
}
