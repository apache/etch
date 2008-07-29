package etch.bindings.java.transport;

import java.net.Socket;

import etch.bindings.java.support.DeliveryService;
import etch.util.Resources;
import etch.util.core.io.Packetizer;
import etch.util.core.io.Session;
import etch.util.core.io.SessionListener;
import etch.util.core.io.TcpConnection;
import etch.util.core.io.TcpListener;
import etch.util.core.io.TlsConnection;
import etch.util.core.io.TlsListener;
import etch.util.core.io.Transport;
import etch.util.core.io.TransportData;


/**
 * Transport factory for tcp.
 */
public class TcpTransportFactory extends TransportFactory
{
	private boolean isSecure = false;
	
	public TcpTransportFactory(boolean isSecure) 
	{
		this.isSecure = isSecure;		
	}
	
	public TcpTransportFactory()
	{
		this(false);
	}
	
	
	public DeliveryService newTransport( String uri,
		Resources resources ) throws Exception
	{
		Socket socket = (Socket) resources.get( "SOCKET" );
		
		TransportData c = null;
		
		if (isSecure)
			c = new TlsConnection( socket, uri, resources );
		else
			c = new TcpConnection( socket, uri, resources );	
		
		Packetizer p = new Packetizer( c, uri, resources );
		
		Messagizer m = new Messagizer( p, uri, resources );
		
		PlainMailboxManager r = new PlainMailboxManager( m, uri, resources );
		
		DefaultDeliveryService d = new DefaultDeliveryService( r, uri, resources );
		
		return d;
	}

	@Override
	public Transport<?> newListener( final String uri,
		final Resources resources, final ServerFactory factory )
		throws Exception
	{
		
		
		SessionListener lh = new SessionListener()
		{
			public void sessionAccepted( Socket socket ) throws Exception
			{
				Resources r = new Resources( resources );
				r.put( "SOCKET", socket );
				
				DeliveryService d = newTransport( uri, r );
				
				factory.newServer( d );
				
				d.transportControl( Transport.START, null );
			}

			public Object sessionQuery( Object query ) throws Exception
			{
				if (factory instanceof Session)
					return ((Session) factory).sessionQuery( query );
				else
					throw new UnsupportedOperationException( "unknown query "+query );
			}

			public void sessionControl( Object control, Object value ) throws Exception
			{
				if (factory instanceof Session)
					((Session) factory).sessionControl( control, value );
				else
					throw new UnsupportedOperationException( "unknown control "+control );
			}

			public void sessionNotify( Object event ) throws Exception
			{
				if (factory instanceof Session)
					((Session) factory).sessionQuery( event );
			}
		};
		
		Transport l;

		if (!isSecure) {
			l = new TcpListener( uri, resources );
		}
		else {
			l = new TlsListener(uri,resources);
		}
		l.setSession( lh );
			
		return l;
	}
}
