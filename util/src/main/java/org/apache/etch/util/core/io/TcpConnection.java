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
import java.io.InputStream;
import java.net.ConnectException;
import java.net.Socket;
import java.net.SocketException;

import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;


/**
 * Implementation of connection which handles a tcp connection.
 */
public class TcpConnection extends TcpTransport
{
	/**
	 * Constructs the Connection from a uri.
	 * @param socket
	 * @param uri
	 * @param resources
	 */
	public TcpConnection( Socket socket, String uri, Resources resources )
	{
		this( socket, new URL( uri ), resources );
	}
	
	/**
	 * Constructs the Connection from a uri.
	 * @param socket
	 * @param uri
	 * @param resources
	 */
	public TcpConnection( Socket socket, URL uri, Resources resources )
	{
		super( uri, resources );
		
		if (socket == null)
		{
			String h = uri.getHost();
			if (h == null)
				throw new IllegalArgumentException( "host == null" );
			
			Integer p = uri.getPort();
			if (p == null)
				throw new IllegalArgumentException( "port == null" );
			
			if (p <= 0 || p > 65535)
				throw new IllegalArgumentException( "port <= 0 || port > 65535" );
			
			this.socket = null;
			host = h;
			port = p;
		}
		else
		{
			this.socket = socket;
			host = null;
			port = 0;
		}
	}

	private final String host;
	
	private final int port;
	
	@Override
	public String toString()
	{
		Socket s = socket;
		
		if (s != null)
			return String.format(
				"TcpConnection(up, %s:%d, %s:%d)",
				s.getLocalAddress(), s.getLocalPort(),
				s.getInetAddress(), s.getPort() );
		
		return String.format( "TcpConnection(down, %s:%d)", host, port );
	}

	@Override
	protected boolean isServer()
	{
		return host == null;
	}

	@Override
	protected Socket newSocket() throws Exception
	{
		try
		{
			return new Socket( host, port );
		}
		catch ( ConnectException e )
		{
			ConnectException x = new ConnectException( "Connection refused: "+host+":"+port );
			x.initCause( e );
			throw x;
		}
	}

	@Override
	protected void readSocket() throws Exception
	{
		final InputStream is = inputStream;
		if (is == null)
			throw new IOException( "socket closed" );
		
		// TODO allow setting input buffer size.
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
}
