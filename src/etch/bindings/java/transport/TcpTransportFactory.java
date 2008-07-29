package etch.bindings.java.transport;

import java.net.Socket;

import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.core.io.Packetizer;
import metreos.core.io.TcpConnection;
import metreos.util.URL;

/**
 * Transport factory for tcp.
 */
public class TcpTransportFactory extends TransportFactory
{
	public DefaultDeliveryService newTransport( URL uri,
		Resources resources ) throws Exception
	{
		Socket socket = (Socket) resources.get( "SOCKET" );
		
		TcpConnection c = new TcpConnection( uri, resources, socket );
		
		Packetizer p = new Packetizer( uri, resources );
		c.setHandler( p );
		
		Messagizer m = new Messagizer( uri, resources );
		p.setHandler( m );
		
		MailboxManager r = new MailboxManager( uri, resources );
		m.setHandler( r );
		
		DefaultDeliveryService d = new DefaultDeliveryService( uri, resources );
		r.setHandler( d );
		
		return d;
	}

	@Override
	public Transport newListener( final URL uri,
		final Resources resources, final ServerFactory factory )
		throws Exception
	{
		ListenerHandler lh = new ListenerHandler()
		{
			public void accepted( Socket s ) throws Exception
			{
				Resources r = new Resources( resources );
				r.put( "SOCKET", s );
				r.put( Messagizer.VALUE_FACTORY, factory.newValueFactory() );
				
				DefaultDeliveryService d = newTransport( uri, r );
				
				factory.newServer( d );
				
				d.transportControl( Transport.START, null );
			}

			public Listener getSource()
			{
				return src;
			}

			public void setSource( Listener src )
			{
				this.src = src;
			}
			
			private Listener src;

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

		Listener l = new Listener( uri );
		l.setHandler( lh );
		return l;
	}
}
