// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package metreos.core.io;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;

import metreos.core.Who;
import metreos.util.FlexBuffer;
import metreos.util.URL;
import etch.bindings.java.transport.Resources;

/**
 * Implementation of connection which handles a udp connection.
 */
public class UdpConnection extends Connection<PacketHandler>
	implements PacketSource
{
	/**
	 * Constructs the connection using an existing socket.
	 * @param handler target to receive packet events.
	 * @param socket existing socket.
	 */
	public UdpConnection( PacketHandler handler, DatagramSocket socket )
	{
		super( handler );
		
		if (socket == null)
			throw new IllegalArgumentException( "socket == null" );
		
		this.socket = socket;
	}
	
	private final DatagramSocket socket;

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
	public UdpConnection( URL uri, Resources resources )
		throws Exception
	{
		super( null );
		
		String host = uri.getHost();
		if (host == null)
			throw new IllegalArgumentException( "host == null" );
		
		host = translateHost( host );
		
		Integer port = uri.getPort();
		if (port == null)
			throw new IllegalArgumentException( "port == null" );
		
		if (port <= 0 || port >= 65536)
			throw new IllegalArgumentException( "port <= 0 || port >= 65536" );
		
		boolean listen = uri.getBooleanTerm( "listen", false );
		
		Integer defTrafficClass = uri.getIntegerTerm( TRAFFIC_CLASS );
		if (defTrafficClass != null)
			setDefaultTrafficClass( defTrafficClass );
		
		// there are three ways to do this:
		// 1. bound socket, unconnected (listening to anyone on well-known host/port
		// 2. bound socket, connected (listening to someone on well-known host/port)
		// 3. unbound socket, connected (listening to someone on assigned host/port)
		
		socket = new DatagramSocket();
		
		if (listen)
			socket.bind( host != null ? new InetSocketAddress( host, port ) : new InetSocketAddress( port ) );
		else
			socket.connect( new InetSocketAddress( host, port ) );
	}
	
	// TODO assign defaultRecipient a value somehow
	private Who defaultRecipient;
	
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

	@Override
	protected synchronized boolean openSocket( boolean reconnect ) throws Exception
	{
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
					handler.packet( null, buf );
				else
					handler.packet( getWho( p.getAddress(), p.getPort() ), buf );
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

	public void packet( Who recipient, FlexBuffer buf ) throws IOException
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
		socket.close();
	}

	public SocketAddress localAddress()
	{
		return socket.getLocalSocketAddress();
	}

	public SocketAddress remoteAddress()
	{
		return socket.getRemoteSocketAddress();
	}

//	public Object oobQuerySource( Object query ) throws Exception
//	{
//		return super.oobQuerySource( query );
//	}

//	public void oobControlSource( Object control, Object value ) throws Exception
//	{
//		super.oobControlSource( control, value );
//	}

//	public void oobNotifySource( Object event ) throws Exception
//	{
//		super.oobNotifySource( event );
//	}
}