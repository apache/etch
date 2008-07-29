using System;
using System.Text;
using System.Net;
using System.Net.Sockets;
using System.Collections.Generic;
using Etch.Msg;

namespace Etch.Transport
{
    /// <summary>
    /// Default listener handler. Accepts connections and constructs stubs
    /// and value factory to handle it.
    /// </summary>
    public abstract class DefaultListenerHandler : ListenerHandler
    {

        public void Accepted( Socket s ) 
	    {
		    ValueFactory vf = NewValueFactory();
    		
		    MessageHandler<MailboxMessageSource> mh = NewMessageHandler();
    		
		    MailboxManager mgr = new MailboxManager( mh, 0 );
            mh.SetSource( mgr );

		    Messagizer m = new Messagizer( mgr, vf );
		    mgr.SetSource( m );
    		
		    Packetizer p = new Packetizer( m, Packetizer.DEFAULT_MAX_PKT_SIZE);
		    m.SetSource( p );
    		
		    TcpConnection c = new TcpConnection( p, s );
		    p.SetSource( c );

            c.Start();
	    }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>an appropriate value factory for this connection.</returns>
	    abstract protected ValueFactory NewValueFactory();

        /// <summary>
        /// 
        /// </summary>
        /// <returns>an appropriate message handler for this connection.</returns>
	    abstract protected MessageHandler<MailboxMessageSource> NewMessageHandler();

        public abstract override string ToString();

        public Source GetSource()
        {
            return src;
        }

        public void SetSource( Source src )
        {
            this.src = ( Listener ) src;
        }

        private Listener src;

        public Object SessionQuery( Object query )
        {
            throw new Exception( "unknown query " + query );
        }

        public void SessionControl( Object control, Object value )
        {
            throw new Exception( "unknown control " + control );
        }

        public void SessionNotify( Object eventObj )
        {
            //ignore
        }
    }
}
