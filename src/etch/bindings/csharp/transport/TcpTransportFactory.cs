using System;
using System.Collections.Generic;
using System.Text;
using System.Net.Sockets;
using Etch.Support;
using Etch.Util;

namespace Etch.Transport
{
    public class TcpTransportFactory : TransportFactory
    {

        public override DefaultDeliveryService NewTransport( Etch.Util.URL uri, Resources resources )
        {
            Socket socket = ( Socket ) resources.Get( "SOCKET" );

            TcpConnection c = new TcpConnection( uri, resources, socket );

            Packetizer p = new Packetizer( uri, resources );
            c.SetHandler( p );

            Messagizer m = new Messagizer( uri, resources );
            p.SetHandler( m );

            MailboxManager r = new MailboxManager( uri, resources );
            m.SetHandler( r );

            DefaultDeliveryService d = new DefaultDeliveryService( uri, resources );
            r.SetHandler( d );

            return d;
        }

        public override Transport NewListener( URL uri, Resources resources, ServerFactory factory )
        {
            MyListenerHandler lh = new MyListenerHandler( this, uri, resources, factory );
            Listener l = new Listener( uri );
            l.SetHandler( lh );
            return l;
        }

        public class MyListenerHandler : ListenerHandler
        {
            private TcpTransportFactory _ttf;
            private Resources _resources;
            private URL _uri;
            private ServerFactory _factory;

            public MyListenerHandler( TcpTransportFactory ttf, URL uri, Resources resources, ServerFactory factory )
            {
                _ttf = ttf;
                _resources = resources;
                _uri = uri;
                _factory = factory;
            }

            #region ListenerHandler Members

            public void Accepted( Socket s )
            {
                Resources r = new Resources( _resources );
                r.Add( "SOCKET", s );
                r.Add( Messagizer.VALUE_FACTORY, _factory.NewValueFactory() );

                DefaultDeliveryService d = _ttf.NewTransport( _uri, r );

                _factory.NewServer( d );

                d.TransportControl( TransportConsts.START, null );
            }   

            #endregion

            #region SourceHandler<DataSource> Members

            private Listener _src;

            public Source GetSource()
            {
                return _src;
            }

            public void SetSource( Source src )
            {
                _src = (Listener) src;
            }

            #endregion

            #region Session Members

            public object SessionQuery( object query )
            {
                if ( _factory is Session )
                    return ( ( Session ) _factory ).SessionQuery( query );
                else
                    throw new Exception( "unknown query " + query );
            }

            public void SessionControl( object control, object value )
            {
                if ( _factory is Session )
                    ( ( Session ) _factory ).SessionControl( control, value );
                else
                    throw new Exception( "unknown control " + control );
            }

            public void SessionNotify( object eventObj )
            {
                if ( _factory is Session )
                    ( ( Session ) _factory ).SessionNotify( eventObj );
            }

            #endregion
        }

    }

    
}
