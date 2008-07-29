using System;
using System.Text;
using System.Collections.Generic;
using Etch.Msg;
using Etch.Support;

namespace Etch.Transport
{
    abstract public class DefaultDeliveryService<T> : AbstractDeliveryService, MessageHandler<MailboxMessageSource> where T : StubBase
    {

        private MailboxMessageSource src;
    	
	    public DefaultDeliveryService()
	    {
		    // Nothing to do.
	    }
    
        /// <summary>
        /// Constructs the MyDeliveryService
        /// </summary>
        /// <param name="src"></param>
	    public DefaultDeliveryService( MailboxMessageSource src )
	    {
		    this.src = src;
	    }

        abstract protected T NewStub( MailboxMessageSource src );

        /*
        /// <summary>
        /// 
        /// </summary>
        /// <param name="src"></param>
        /// Exception:
        ///     throws Exception 
        public void Down(MailboxMessageSource src) 
	    {
            Console.WriteLine( "messageHandlerDown", "who", this, "src", src );
		    //Log.Report( "messageHandlerDown", "who", this, "src", src );

            if ( stub != null )
            {
                stub.Message( src, null, null );
                stub = null;
            }
    	}

        /// <summary>
        /// 
        /// </summary>
        /// <param name="src"></param>
        /// Exception:
        ///     throws Exception 
        public void Up(MailboxMessageSource src) 
	    {
            Console.WriteLine( "messageHandlerUp", "who", this, "src", src );
		    //Log.Report( "messageHandlerUp", "who", this, "src", src );
		    stub = NewStub( src );
    	}
        */



        public static DefaultDeliveryService<T> SetUp( DefaultDeliveryService<T> dds,
            DataSource c, ValueFactory vf )
        {
            MailboxManager mgr = new MailboxManager( dds, 0 );

            Messagizer m = new Messagizer( mgr, vf );
            mgr.SetSource( m );

            Packetizer p = new Packetizer( m, Packetizer.DEFAULT_MAX_PKT_SIZE );
            m.SetSource( p );
            p.SetSource( c );

            dds.SetSource( mgr );
            c.SetHandler( p );

            return dds;
        }

        public void SessionNotify( Object eventObj )
        {
            if ( eventObj.Equals( SessionConsts.UP ) )
                AddStub( NewStub( GetSource() ) );

            base.SessionNotify( eventObj );

            if ( eventObj.Equals( SessionConsts.DOWN ) )
                RemoveStub();
        }
    }
}
