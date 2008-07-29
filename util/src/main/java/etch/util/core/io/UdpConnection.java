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

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;

import etch.util.FlexBuffer;
import etch.util.Resources;
import etch.util.URL;
import etch.util.core.Who;

/**
 * Implementation of connection which handles a udp connection.
 */
public class UdpConnection extends Connection<SessionPacket>
	implements TransportPacket
{
	/**
	 * Term on the uri which specifies the default traffic class.
	 * @see #setDefaultTrafficClass(int)
	 */
	private static final String TRAFFIC_CLASS = "UdpConnection.trafficClass";
	
	/**
	 * Constructs the Connection from a uri.
	 * @param uri
	 * @param resources
	 * @throws Exception
	 */
	public UdpConnection( String uri, Resources resources )
		throws Exception
	{
		URL u = new URL( uri );
		
		listen = u.getBooleanTerm( "listen", false );
		
		host = translateHost( u.getHost() );
		if (!listen && host == null)
			throw new IllegalArgumentException( "host == null" );
		
		port = u.getPort();
		if (port == null)
			throw new IllegalArgumentException( "port == null" );
		
		if (port <= 0 || port > 65535)
			throw new IllegalArgumentException( "port <= 0 || port > 65535" );
		
		setDefaultTrafficClass( u.getIntegerTerm( TRAFFIC_CLASS, 0 ) );
		
		// there are three ways to do this:
		// 1. bound socket, unconnected (listening to anyone on well-known host/port
		// 2. bound socket, connected (listening to someone on well-known host/port)
		// 3. unbound socket, connected (listening to someone on assigned host/port)
	}
	
	private final String host;
	
	private final Integer port;
	
	private final boolean listen;
	
	private DatagramSocket socket;
	
	// TODO assign defaultRecipient a value somehow
	private Who defaultRecipient;
	
	/**
	 * Sets the default value for the traffic class (or type of service
	 * or dscp). For example, call signaling traffic is classified as
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

	@Override
	protected synchronized boolean openSocket( boolean reconnect ) throws Exception
	{
		if (socket != null)
			socket.close();
		
		socket = new DatagramSocket();
		
		if (listen)
			socket.bind( host != null ? new InetSocketAddress( host, port ) : new InetSocketAddress( port ) );
		else
			socket.connect( new InetSocketAddress( host, port ) );
		
		if (!reconnect && socket != null)
			return true;
		
		return false;
	}

	@Override
	protected void setupSocket() throws Exception
	{
		DatagramSocket s = checkSocket();
		
		s.setTrafficClass( trafficClass );
	}

	@Override
	protected void readSocket() throws Exception
	{
		final DatagramSocket s = checkSocket();
		// TODO allow buffer size to be specified.
		final FlexBuffer buf = new FlexBuffer( new byte[8192] );
		final DatagramPacket p = new DatagramPacket( new byte[0], 0 );
		
		try
		{
			while (isStarted())
			{
				p.setData( buf.getBuf() );
				
				s.receive( p );
				
				buf.setIndex( 0 );
				buf.setLength( p.getLength() );
				
				if (s.isConnected())
					session.sessionPacket( null, buf );
				else
					session.sessionPacket( getWho( p.getAddress(), p.getPort() ), buf );
			}
		}
		catch ( SocketException e )
		{
			if ("socket closed".equalsIgnoreCase( e.getMessage()))
				return;
			throw e;
		}
	}

	private DatagramSocket checkSocket() throws IOException
	{
		DatagramSocket s = socket;
		if (s == null || s.isClosed())
			throw new SocketException( "socket closed" );
		return s;
	}
	
	private Who getWho( InetAddress address, int port )
	{
		if (lastWho == null || !lastWho.matches( address, port ))
			lastWho = new InetWho( address, port );
		return lastWho;
	}
	
	private InetWho lastWho;
	
	private final DatagramPacket outp = new DatagramPacket( new byte[0], 0 );

	public void transportPacket( Who recipient, FlexBuffer buf ) throws IOException
	{
//		System.out.printf( "packet( %s, buf( %d, %d ))", recipient, buf.index(), buf.avail() );
		if (socket.isConnected())
		{
			outp.setData( buf.getBuf(), buf.index(), buf.avail() );
			socket.send( outp );
			return;
		}
		
		if (recipient == null)
			recipient = defaultRecipient;
		
		if (recipient != null)
		{
			InetWho iw = (InetWho) recipient;
			outp.setData( buf.getBuf(), buf.index(), buf.avail() );
			outp.setAddress( iw.getInetAddress() );
			outp.setPort( iw.getPort() );
			socket.send( outp );
			
			return;
		}
		
		throw new IllegalArgumentException( "no recipient" );
	}

	public int headerSize()
	{
		return 0;
	}

	@Override
	public void close( boolean reset )
	{
		DatagramSocket s = socket;
		if (s != null)
		{
			socket = null;
			s.close();
		}
	}

	@Override
	public SocketAddress localAddress()
	{
		return socket.getLocalSocketAddress();
	}

	@Override
	public SocketAddress remoteAddress()
	{
		return socket.getRemoteSocketAddress();
	}
}