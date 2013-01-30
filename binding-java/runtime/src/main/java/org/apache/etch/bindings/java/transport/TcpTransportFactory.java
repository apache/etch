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

import java.net.Socket;

import javax.net.ssl.SSLSocket;

import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.bindings.java.support.TransportFactory;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.io.Packetizer;
import org.apache.etch.util.core.io.SessionListener;
import org.apache.etch.util.core.io.TcpConnection;
import org.apache.etch.util.core.io.TcpListener;
import org.apache.etch.util.core.io.TlsConnection;
import org.apache.etch.util.core.io.TlsListener;
import org.apache.etch.util.core.io.Transport;
import org.apache.etch.util.core.io.TransportData;
import org.apache.etch.util.core.io.TransportPacket;


/**
 * Transport factory for tcp or tls connections.
 */
public class TcpTransportFactory extends TransportFactory
{
	/**
	 * Constructs a TcpTransportFactory which delivers TcpConnection.
	 */
	public TcpTransportFactory()
	{
		this( false );
	}
	
	/**
	 * Constructs a TcpTransportFactory which delivers TcpConnection or a
	 * TlsConnection depending upon the isSecure parameter.
	 * @param isSecure true if TlsConnection is desired, false otherwise.
	 */
	public TcpTransportFactory( boolean isSecure )
	{
		this.isSecure = isSecure;
	}
	
	private final boolean isSecure;
	
	private final static String SOCKET = "TcpTransportFactory.socket";
	
	@Override
	public TransportMessage newTransport( String uri,
		Resources resources ) throws Exception
	{
		URL u = new URL( uri );
		
		Object socket = resources.get( SOCKET );
		
		TransportData c = null;
		
		if (isSecure)
			c = new TlsConnection( (SSLSocket) socket, u, resources );
		else
			c = new TcpConnection( (Socket) socket, u, resources );
		
		TransportPacket p = new Packetizer( c, u, resources );
		
		TransportMessage m = new Messagizer( p, u, resources );
		
		m = addFilters( m, u, resources );
		
		ValueFactory vf = (ValueFactory) resources.get( Transport.VALUE_FACTORY );
		vf.lockDynamicTypes();
		
		return m;
	}

	@Override
	public Transport<ServerFactory> newListener( final String uri,
		final Resources resources ) throws Exception
	{
		URL u = new URL( uri );
		
		Transport<SessionListener<Socket>> l;

		if (isSecure)
			l = new TlsListener( u, resources );
		else
			l = new TcpListener( u, resources );
		
		return new MySessionListener( l, uri, resources );
	}
	
	private class MySessionListener implements Transport<ServerFactory>, SessionListener<Socket>
	{
		/**
		 * @param transport
		 * @param uri
		 * @param resources
		 */
		public MySessionListener( Transport<SessionListener<Socket>> transport,
			String uri, Resources resources )
		{
			this.transport = transport;
			this.uri = uri;
			this.resources = resources;
			
			transport.setSession( this );
		}
		
		private final Transport<SessionListener<Socket>> transport;
		
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
			return "TcpTransportFactory.MySessionListener/"+transport;
		}

		public Object transportQuery( Object query ) throws Exception
		{
			return transport.transportQuery( query );
		}

		public void transportControl( Object control, Object value )
			throws Exception
		{
			transport.transportControl( control, value );
		}

		public void transportNotify( Object event ) throws Exception
		{
			transport.transportNotify( event );
		}

		public void sessionAccepted( Socket socket )
			throws Exception
		{
			Resources r = new Resources( resources );
			r.put( SOCKET, socket );
			
			ValueFactory vf = session.newValueFactory( uri );
			r.put( Transport.VALUE_FACTORY, vf );
			
			TransportMessage t = newTransport( uri, r );
			
			session.newServer( t, uri, r );
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			return session.sessionQuery( query );
		}

		public void sessionControl( Object control, Object value )
			throws Exception
		{
			session.sessionControl( control, value );
		}

		public void sessionNotify( Object event ) throws Exception
		{
			session.sessionNotify( event );
		}
	}
}
