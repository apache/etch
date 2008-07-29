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
        private bool isSecure = false;

        public TcpTransportFactory(bool isSecure)
        {
            this.isSecure = isSecure;
        }

        public TcpTransportFactory(): this(false)
        {
            
        }

        public override DeliveryService NewTransport( string uri, Resources resources )
        {
            Socket socket = ( Socket ) resources.Get( "SOCKET" );

            TransportData c = null;

            if (isSecure)
                c = new TlsConnection(socket, uri, resources);
            else
                c = new TcpConnection( socket, uri, resources );

            Packetizer p = new Packetizer(c,uri, resources );
            

            Messagizer m = new Messagizer( p,uri, resources );
          

            PlainMailboxManager r = new PlainMailboxManager( m,uri, resources );
          

            DefaultDeliveryService d = new DefaultDeliveryService( r,uri, resources );
         

            return d;
        }

        public override Transport<SessionListener> NewListener( string uri, Resources resources, ServerFactory factory )
        {
            MyListenerHandler lh = new MyListenerHandler( this, uri, resources, factory );

            Transport <SessionListener> l = null;
  /*          if (isSecure)
                l = new TlsListener(uri);
            else */
            l = new Listener( uri );
            l.SetSession( lh );
            return l;
        }

        public class MyListenerHandler : SessionListener
        {
            private TcpTransportFactory _ttf;
            private Resources _resources;
            private string _uri;
            private ServerFactory _factory;

            public MyListenerHandler( TcpTransportFactory ttf, string uri, Resources resources, ServerFactory factory )
            {
                _ttf = ttf;
                _resources = resources;
                _uri = uri;
                _factory = factory;
            }

           

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

            #region SessionListener Members

            public void SessionAccepted(Socket socket)
            {
                Resources r = new Resources(_resources);
                r.Add("SOCKET", socket);


                DeliveryService d = _ttf.NewTransport(_uri, r);

                _factory.NewServer(d);

                d.TransportControl(TransportConsts.START, null);
            }

            #endregion
        }

    }

    
}
