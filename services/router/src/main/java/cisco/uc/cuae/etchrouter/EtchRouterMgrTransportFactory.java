/* $Id$
 *
 * Copyright 2009-2010 Cisco Systems Inc.
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
package cisco.uc.cuae.etchrouter;

import java.net.Socket;

import javax.net.ssl.SSLSocket;

import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.bindings.java.transport.DefaultDeliveryService;
import org.apache.etch.bindings.java.transport.MailboxManager;
import org.apache.etch.bindings.java.transport.Messagizer;
import org.apache.etch.bindings.java.transport.PlainMailboxManager;
import org.apache.etch.bindings.java.transport.TcpTransportFactory;
import org.apache.etch.bindings.java.transport.TransportMessage;
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
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class EtchRouterMgrTransportFactory extends TcpTransportFactory
{
	
	private static EtchRouterMgrTransportFactory _tcpTransportFactory = new EtchRouterMgrTransportFactory(false);
	
	private static EtchRouterMgrTransportFactory _tlsTransportFactory = new EtchRouterMgrTransportFactory(true);
	
	private static EtchRouterMgrTransportFactory getFactoryByScheme( String scheme, String uri ) throws Exception
	{
		EtchRouterMgrTransportFactory f = null;
		if ("tcp".equalsIgnoreCase( scheme ))
			f = _tcpTransportFactory;
		else if ("tls".equalsIgnoreCase( scheme ))
			f = _tlsTransportFactory;
		else
			throw new Exception("No transport factory found for URL " + uri);
		return f;
	}
	
	// TODO these methods should not be here. they come from Transport.
	static public DeliveryService getTransport( String uri,
		Resources resources ) throws Exception
	{
		URL u = new URL( uri );
		EtchRouterMgrTransportFactory f = getFactoryByScheme( u.getScheme(), uri );
		return f.newTransport( uri, resources );
	}
	
	// TODO these methods should not be here. they come from Transport.
	static public Transport<ServerFactory> getListener( String uri,
		Resources resources ) throws Exception
	{
		URL u = new URL( uri );
		EtchRouterMgrTransportFactory f = getFactoryByScheme( u.getScheme(), uri );
		return f.newListener( uri, resources );
	}

	/**
	 * Constructs a TcpTransportFactory which delivers TcpConnection.
	 */
	public EtchRouterMgrTransportFactory()
	{
		this( false );
	}
	
	/**
	 * Constructs a TcpTransportFactory which delivers TcpConnection or a
	 * TlsConnection depending upon the isSecure parameter.
	 * @param isSecure true if TlsConnection is desired, false otherwise.
	 */
	public EtchRouterMgrTransportFactory( boolean isSecure )
	{
		super(isSecure);
		_isSecure = isSecure;
	}
	
	private final static String _SOCKET = "TcpTransportFactory.socket";
	
	private boolean _isSecure = false;
	
	@Override
	public DeliveryService newTransport( String uri, Resources resources ) throws Exception
	{
		EtchRouterManager etchRouterManager = (EtchRouterManager)resources.get( EtchRouterManager._ETCH_ROUTER_MANAGER );
		
		URL u = new URL( uri );
		
		Object socket = resources.get( _SOCKET );
		
		TransportData c = null;
		
		if (_isSecure)
			c = new TlsConnection( (SSLSocket) socket, u, resources );
		else
			c = new TcpConnection( (Socket) socket, u, resources );
		
		TransportPacket p = new Packetizer( c, u, resources );
		
		Messagizer msg = new Messagizer( p, u, resources );
		
		TransportMessage m = addFilters( msg, u, resources );
		
		EtchRouterMessageFilter messageFilter = new EtchRouterMessageFilter(m, u, resources);
		
		MailboxManager r = new PlainMailboxManager( messageFilter, u, resources );
		
		DeliveryService d = new DefaultDeliveryService( r, u, resources );
		
		ValueFactory vf = (ValueFactory) resources.get( Transport.VALUE_FACTORY );
		
		ConnectionStackInfo connInfo = new ConnectionStackInfo( messageFilter, r, d, vf );
		
		etchRouterManager.addConnectionStackInfo( connInfo );
		
		return d;
	}

	@SuppressWarnings("unchecked")
	@Override
	public Transport<ServerFactory> newListener( final String uri, final Resources resources )
		throws Exception
	{
		URL u = new URL( uri );
		
		Transport<SessionListener<Socket>> l;

		if (_isSecure)
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
		public MySessionListener( Transport<SessionListener<Socket>> transport, String uri, Resources resources )
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

		public void sessionAccepted( Socket socket ) throws Exception
		{
			Resources r = new Resources( resources );
			r.put( _SOCKET, socket );
			
			ValueFactory vf = session.newValueFactory();
			r.put( Transport.VALUE_FACTORY, vf );
			
			TransportMessage t = newTransport( uri, r );
			
			session.newServer( uri, r, t );
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
