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
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.util.concurrent.BlockingQueue;

import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;

public class UdpConnection extends Connection<SessionPacket> implements TransportPacket
{
	public final static String BROADCAST = "UdpConnection.broadcast";

	private SocketAddress remoteAddress;
	private final UdpListener listener;
	private BlockingQueue<DatagramPacket> readQueue;

	private final int delay;
	private final boolean enableBroadcast;
	private final String host;
	private final int port;

	private DatagramSocket socket = null;
	private InetWho lastWho = null;
	private DatagramPacket sendDatagramPacket = new DatagramPacket( new byte[0], 0 );

	public UdpConnection(SocketAddress remoteAddress, UdpListener listener)
	{
		this.remoteAddress = remoteAddress;
		this.listener = listener;

		readQueue = listener.allocReadQueue( remoteAddress );

		delay = 0;
		enableBroadcast = false;
		host = null;
		port = 0;
	}

	public UdpConnection(String uri)
	{
		this( new URL( uri ) );
	}

	public UdpConnection(URL uri)
	{
		this( translateHost( uri.getHost() ), uri.getPort(), uri.getBooleanTerm( BROADCAST, false ), 0 );
	}

	public UdpConnection(String host, Integer port, boolean enableBroadcast, int delay)
	{
		if (host == null)
			throw new IllegalArgumentException( "host is missing" );
		else if (host.equals( "255.255.255.255" ))
			enableBroadcast = true;

		if (port == null)
			throw new IllegalArgumentException( "port" );
		else if (port <= 0 || port >= 65536)
			throw new IllegalArgumentException( "port <= 0 || port >= 65536" );

		this.host = host;
		this.port = port;
		this.enableBroadcast = enableBroadcast;
		this.delay = delay;

		listener = null;
	}

	private Who getWho( InetAddress address, int port )
	{
		if (lastWho == null || !lastWho.matches( address, port ))
			lastWho = new InetWho( address, port );

		return lastWho;
	}

	@Override
	protected synchronized boolean openSocket( boolean reconnect ) throws Exception
	{
		// if a one time connection from a server socket listener, just
		// return the existing socket. Bail if this is a reconnect.
		if (listener != null)
		{
			if (!reconnect && readQueue == null)
				readQueue = listener.allocReadQueue( remoteAddress );

			return !reconnect;
		}

		// if a reconnect but no retries allowed, then bail.
		if (reconnect && delay == 0)
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
				remoteAddress = new InetSocketAddress( host, port );
				socket = new DatagramSocket();
				socket.connect(remoteAddress);
				return true;
			}
			catch (Exception e)
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
		if (socket != null)
			socket.setBroadcast( enableBroadcast );
	}

	@Override
	protected void readSocket() throws Exception
	{
		FlexBuffer flexBuffer = null;
		DatagramPacket datagramPacket = null;

		if (socket != null)
		{
			// TODO allow buffer size to be specified.
			final byte[] buf = new byte[8192];
			datagramPacket = new DatagramPacket( buf, buf.length );
			flexBuffer = new FlexBuffer( datagramPacket.getData() );
		}

		try
		{
			while (isStarted())
			{
				if (readQueue != null)
				{
					datagramPacket = readQueue.take();
					if (datagramPacket == UdpListener.READ_QUEUE_SENTINEL)
						break;

					flexBuffer = new FlexBuffer( datagramPacket.getData() );
				}
				else {
					socket.receive( datagramPacket );

					flexBuffer.setIndex( 0 );
					flexBuffer.setLength( datagramPacket.getLength() );
				}

				Who sender = getWho( datagramPacket.getAddress(), datagramPacket.getPort() );

				session.sessionPacket( sender, flexBuffer );
			}
		}
		catch (SocketException e)
		{
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

	@Override
	public void close( boolean reset )
	{
		if (listener != null) {
			listener.releaseReadQueue( remoteAddress );
			readQueue = null;
		}

		DatagramSocket s = socket;
		if (socket != null)
		{
			socket = null;
			s.close();
		}
	}

	public int headerSize()
	{
		return 0;
	}

	public void transportPacket( Who recipient, FlexBuffer buf ) throws IOException
	{
		SocketAddress recipientAddress = remoteAddress;
		
		if (recipient != null && recipient instanceof InetWho) {
			InetWho inetWho = (InetWho)recipient;

			recipientAddress = new InetSocketAddress( inetWho.getInetAddress(), inetWho.getPort() );
		}

		sendDatagramPacket.setData( buf.getBuf(), buf.index(), buf.avail() );
		sendDatagramPacket.setSocketAddress( recipientAddress );

		if (listener != null)
			listener.Send(sendDatagramPacket);
		else
			socket.send(sendDatagramPacket);
	}

	@Override
	public String toString()
	{
		DatagramSocket s = socket;

		if (listener != null || s != null)
			try
			{
				InetSocketAddress localAddress = (InetSocketAddress) localAddress();
				InetSocketAddress remoteAddress = (InetSocketAddress) remoteAddress();
				String state = "up";
				
				if (listener != null && readQueue == null)
					state = "down";

				return String.format( "UdpConnection(%s, %s:%d, %s:%d)",
					state,
					localAddress.getAddress(), localAddress.getPort(),
					remoteAddress.getAddress(), remoteAddress.getPort() );
			}
			catch (IOException e)
			{
				// nothing to do
			}

		return String.format( "UdpConnection(down, %s:%d)", host, port );
	}

	@Override
	public SocketAddress localAddress() throws IOException
	{
		if (listener != null)
			return listener.localAddress();

		return socket.getLocalSocketAddress();
	}

	@Override
	public SocketAddress remoteAddress() throws IOException
	{
		if (listener != null)
			return remoteAddress;

		return socket.getRemoteSocketAddress();
	}
}
