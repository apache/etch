using System;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Support;
using Etch.Transport;

namespace etch.examples.benchmark.etch
{
    public class PerfHelper
    {
        public static ListenerHandler newPerfListener( Pool queuedPool, Pool freePool )
        {
            return new MyListenerHandler( queuedPool, freePool );
        }

        public static PerfServer newPerfServer( TcpConnection conn,
            PerfClient client, Pool queued, Pool free )
        {
            DefaultDeliveryService<StubPerfClient> dds = 
                new MyDeliveryService( client, queued, free );

            DefaultDeliveryService<StubPerfClient> svc = 
                DefaultDeliveryService<StubPerfClient>.SetUp( dds, conn, new ValueFactoryPerf() );

            return new RemotePerfServer( svc );
        }


        public class MyListenerHandler : DefaultListenerHandler
        {
            Pool _free;
            Pool _pool;

            public MyListenerHandler( Pool pool, Pool free )
            {
                _free = free;
                _pool = pool;
            }

            protected override Etch.Transport.MessageHandler<MailboxMessageSource> NewMessageHandler()
            {
                return new MyMessageHandler( _pool, _free );
            }

            protected override ValueFactory NewValueFactory()
            {
                return new ValueFactoryPerf();
            }

            public override string ToString()
            {
                return "PerfServerListener";
            }
        }

        public class MyMessageHandler : DefaultMessageHandler<StubPerfServer>
        {
            Pool _free;
            Pool _pool;

            public MyMessageHandler( Pool pool, Pool free )
            {
                _free = free;
                _pool = pool;

            }

            protected override StubPerfServer NewStub( MailboxMessageSource src )
            {
             
                return new StubPerfServer( new ImplPerfServer(), _pool, _free );
            }

            public override string ToString()
            {
                return "PerfServerMessageHandler";
            }
        }

        public class MyDeliveryService : DefaultDeliveryService<StubPerfClient>
        {
            PerfClient _client;
            Pool _free;
            Pool _queued;

            public MyDeliveryService( MailboxMessageSource src )
                : base( src )
            { }

            public MyDeliveryService( PerfClient client, Pool queued, Pool free )
            {
                _client = client;
                _free = free;
                _queued = queued;
            }

            protected override StubPerfClient NewStub( MailboxMessageSource src )
            {
                return new StubPerfClient( _client, _queued, _free );
            }

            public override string ToString()
            {
                return null;
            }
        }
    }
}
