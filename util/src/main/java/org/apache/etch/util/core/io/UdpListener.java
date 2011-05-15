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
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetSocketAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.util.Collections;
import java.util.Hashtable;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

import org.apache.etch.util.URL;

/**
 * Implementation of a connection which handles a socket listener.
 */
public class UdpListener extends Connection<SessionListener<SocketAddress>> implements
		Transport<SessionListener<SocketAddress>>
{
	/**
	 * Query term for URI to specify queue size value for each UdpConnection.
	 * The value is "UdpListener.queueSize".
	 * 
	 * Example: udp://host:port?UdpListener.queueSize=15
	 * 
	 * @see #UdpListener(String)
	 * @see UdpConnection
	 */
	public final static String QUEUE_SIZE = "UdpListener.queueSize";
	
	public static final String REUSE_PORT = "Connection.reusePort";

	public final static DatagramPacket READ_QUEUE_SENTINEL = new DatagramPacket(new byte[0], 0);

	private final String host;
	private final int port;
	private final boolean reusePort;
	private final int queueSize;

	private DatagramSocket socket;
	private Hashtable<SocketAddress, BlockingQueue<DatagramPacket>> readQueues;

	/**
	 * Constructs a UdpListener, initialized from the URI. Listens on the host
	 * and port specified in the URI. To listen on all interfaces, specify host
	 * as ALL_INTFS ("0.0.0.0"). If port is specified or defaulted to 0, an
	 * unused port will be selected.
	 * 
	 * @param uri
	 */
	public UdpListener(String uri)
	{
		this( new URL( uri ) );
	}

	/**
	 * Constructs a UdpListener, initialized from the URI. Listens on the host
	 * and port specified in the URI. To listen on all interfaces, specify host
	 * as ALL_INTFS ("0.0.0.0"). If port is specified or defaulted to 0, an
	 * unused port will be selected.
	 * 
	 * @param uri
	 * @see URL#getHost()
	 * @see URL#getPort()
	 * @see URL#getBooleanTerm(String, boolean)
	 * @see #REUSE_PORT
	 * @see URL#getIntegerTerm(String, int)
	 * @see #QUEUE_SIZE
	 * @see #ALL_INTFS
	 * @see #localAddress()
	 */
	public UdpListener(URL uri)
	{
		this( translateHost( uri.getHost() ), uri.getPort(), uri.getBooleanTerm( REUSE_PORT, false ),
			uri.getIntegerTerm( QUEUE_SIZE, 15 ) );
	}

	/**
	 * Constructs the UdpListener.
	 * 
	 * @param host address to listen to. Null means any local address.
	 * @param port port to listen on. Port must be >= 0. Port of 0 means
	 * allocate an available port.
	 * @param reusePort pass reuse port flag to the socket.
	 * @param queueSize UdpConnection packet queue size.
	 */
	public UdpListener(String host, int port, boolean reusePort, int queueSize)
	{
		if (port < 0 || port > 65535)
			throw new IllegalArgumentException( "port < 0 || port > 65535" );

		if (queueSize <= 0)
			throw new IllegalArgumentException( "queueSize < 0" );

		this.host = host;
		this.port = port;
		this.reusePort = reusePort;
		this.queueSize = queueSize;

		readQueues = new Hashtable<SocketAddress, BlockingQueue<DatagramPacket>>();
	}

	public synchronized void Send( DatagramPacket outp ) throws IOException
	{
		if (socket != null)
			socket.send( outp );
	}

	public BlockingQueue<DatagramPacket> allocReadQueue( SocketAddress address )
	{
		BlockingQueue<DatagramPacket> readQueue = null;

		synchronized (readQueues)
		{
			readQueue = readQueues.get( address );
			if (readQueue == null)
			{
				readQueue = new LinkedBlockingQueue<DatagramPacket>( queueSize );
				readQueues.put( address, readQueue );
			}
		}

		return readQueue;
	}

	private BlockingQueue<DatagramPacket> getReadQueue( SocketAddress address )
	{
		return readQueues.get( address );
	}

	public void releaseReadQueue( SocketAddress address )
	{
		try
		{
			BlockingQueue<DatagramPacket> readQueue = readQueues.remove( address );

			if (readQueue != null)
			{
				readQueue.clear();
				readQueue.put( READ_QUEUE_SENTINEL );
			}
		}
		catch (InterruptedException e)
		{
			// ignore
		}
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
				InetSocketAddress localAddress = host == null ? new InetSocketAddress( port ) : new InetSocketAddress( host, port );
				try
				{
					socket = new DatagramSocket( null );
					socket.setReuseAddress( reusePort );
					socket.bind( localAddress );
					return true;
				}
				catch (BindException e)
				{
					BindException x = new BindException( "Cannot assign requested address: " + host + ":" + port );
					x.initCause( e );
					throw x;
				}
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
		// nothing to do.
	}

	@Override
	protected void readSocket() throws Exception
	{
		while (isStarted())
			try
			{
				byte[] buf = new byte[8192];
				DatagramPacket datagramPacket = new DatagramPacket( buf, buf.length );
				BlockingQueue<DatagramPacket> readQueue;

				socket.receive( datagramPacket );

				readQueue = getReadQueue( datagramPacket.getSocketAddress() );
				if (readQueue == null)
					session.sessionAccepted( datagramPacket.getSocketAddress() );

				readQueue = getReadQueue( datagramPacket.getSocketAddress() );
				if (readQueue != null)
					readQueue.offer( datagramPacket );
			}
			catch (SocketException e)
			{
				if ("socket closed".equalsIgnoreCase( e.getMessage() ))
					break;
			}
	}

	@Override
	protected void stop0() throws Exception
	{
		close( true );
		super.stop0();
	}

	@Override
	public void close( boolean reset ) throws Exception
	{
		for (SocketAddress address : Collections.list( readQueues.keys() ))
			releaseReadQueue( address );

		DatagramSocket s = socket;
		if (socket != null)
		{
			socket = null;
			s.close();
		}
	}

	@Override
	public String toString()
	{
		DatagramSocket s = socket;

		if (s != null)
			return String.format( "UdpListener(up, %s, %d)", s.getInetAddress(), s.getLocalPort() );

		return String.format( "UdpListener(down, %s, %d)", host, port );
	}

	@Override
	public SocketAddress localAddress() throws IOException
	{
		DatagramSocket s = socket;

		if (s != null)
			return s.getLocalSocketAddress();

		return null;
	}

	@Override
	public SocketAddress remoteAddress()
	{
		// ignore.
		return null;
	}
}
