using System;
using System.Collections.Generic;
using System.Text;
using Etch.Util;
using Etch.Msg;

namespace Etch.Transport
{
    public class DefaultDeliveryService : AbstractDeliveryService, Startable, MessageHandler<MailboxMessageSource>
    {

        public DefaultDeliveryService( Connection<DataHandler> conn )
        {
            _conn = conn;
        }

        public DefaultDeliveryService( URL uri, Resources resources )
        {
            _conn = null;
        }

        private readonly Connection<DataHandler> _conn;

        public static AbstractDeliveryService TcpConnection( String host, int port,
            int delay, MessageHandler<MailboxMessageSource> mh, ValueFactory vf )
        {
            TcpConnection c = new TcpConnection( null, host, port, delay );

            Packetizer p = new Packetizer( null, Packetizer.DEFAULT_MAX_PKT_SIZE );
            //		p.setSource( c );
            c.SetHandler( p );

            Messagizer m = new Messagizer( null, vf );
            //		m.setSource( p );
            p.SetHandler( m );

            MailboxManager r = new MailboxManager( null, 0 );
            //		r.setSource( m );
            m.SetHandler( r );

            DefaultDeliveryService d = new DefaultDeliveryService( c );
            //		d.setSource( r );
            r.SetHandler( d );

            return d;
        }

        #region Startable Members

        public void Start()
        {
            _conn.Start();
        }

        public void Stop()
        {
            _conn.Stop();
        }

        public bool IsStarted()
        {
            return _conn.IsStarted();
        }

        #endregion

        public void WaitUp( int maxDelay )
        {
            _conn.WaitUp( maxDelay );
        }

        public void WaitDown( int maxDelay )
        {
            _conn.WaitDown( maxDelay );
        }

        public override string ToString()
        {
            return null;
        }

    }
}
