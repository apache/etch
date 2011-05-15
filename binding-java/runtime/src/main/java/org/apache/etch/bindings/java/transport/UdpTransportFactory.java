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

package org.apache.etch.bindings.java.transport;

import java.net.SocketAddress;

import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.bindings.java.support.TransportFactory;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.io.SessionListener;
import org.apache.etch.util.core.io.Transport;
import org.apache.etch.util.core.io.TransportPacket;
import org.apache.etch.util.core.io.UdpConnection;
import org.apache.etch.util.core.io.UdpListener;

/**
 * Transport factory for udp connections.
 */
public class UdpTransportFactory extends TransportFactory
{
	/**
	 * Resource tags
	 */
	private final static String UDP_LISTENER = "UdpTransportFactory.udpListener";
	private final static String SOCKET_ADDRESS = "UdpTransportFactory.socketAddress";

	/**
	 * Constructs a UdpTransportFactory which delivers UdpConnection.
	 */
	public UdpTransportFactory()
	{
	}

	@Override
	public TransportMessage newTransport( String uri, Resources resources ) throws Exception
	{
		UdpListener udpListener = (UdpListener) resources.get( UDP_LISTENER );
		SocketAddress socketAddress = (SocketAddress) resources.get( SOCKET_ADDRESS );

		URL url = new URL( uri );
		TransportPacket transportPacket = null;
		
		if (udpListener != null)
			transportPacket = new UdpConnection( socketAddress, udpListener );
		else
			transportPacket = new UdpConnection( uri );

		TransportMessage transportMessage = new Messagizer( transportPacket, url, resources );
		transportMessage = addFilters( transportMessage, url, resources );

		ValueFactory vf = (ValueFactory) resources.get( Transport.VALUE_FACTORY );
		vf.lockDynamicTypes();

		return transportMessage;
	}

	@Override
	public Transport<ServerFactory> newListener( final String uri, final Resources resources ) throws Exception
	{
		UdpListener transportListener = new UdpListener( uri );
		return new MySessionListener( this, transportListener, uri, resources );
	}

	private class MySessionListener implements Transport<ServerFactory>, SessionListener<SocketAddress>
	{
		/**
		 * @param utf
		 * @param transportListener
		 * @param uri
		 * @param resources
		 */
		public MySessionListener(UdpTransportFactory utf, UdpListener transportListener, String uri, Resources resources)
		{
			this.udpTransportFactory = utf;
			this.listener = transportListener;
			this.uri = uri;
			this.resources = resources;

			transportListener.setSession( this );
		}

		private final UdpTransportFactory udpTransportFactory;
		private final UdpListener listener;
		private final String uri;
		private final Resources resources;

		public ServerFactory getSession()
		{
			return session;
		}

		public void setSession( ServerFactory session )
		{
			this.session = session;
		}

		private ServerFactory session;

		@Override
		public String toString()
		{
			return "UdpTransportFactory.MySessionListener/" + listener;
		}

		public Object transportQuery( Object query ) throws Exception
		{
			return listener.transportQuery( query );
		}

		public void transportControl( Object control, Object value ) throws Exception
		{
			listener.transportControl( control, value );
		}

		public void transportNotify( Object event ) throws Exception
		{
			listener.transportNotify( event );
		}

		@Override
		public void sessionAccepted( SocketAddress socketAddress ) throws Exception
		{
			ValueFactory vf = session.newValueFactory( uri );
			Resources r = new Resources( resources );

			r.put( UDP_LISTENER, listener );
			r.put( SOCKET_ADDRESS, socketAddress );
			r.put( Transport.VALUE_FACTORY, vf );

			TransportMessage t = udpTransportFactory.newTransport( uri, r );

			session.newServer( t, uri, r );
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			return session.sessionQuery( query );
		}

		public void sessionControl( Object control, Object value ) throws Exception
		{
			session.sessionControl( control, value );
		}

		public void sessionNotify( Object event ) throws Exception
		{
			session.sessionNotify( event );
		}
	}
}
