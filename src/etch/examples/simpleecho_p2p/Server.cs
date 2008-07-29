using System;
using System.Collections.Generic;
using System.Text;

using Etch.Transport;
using Etch.Msg;

using etch.examples.simpleecho;

namespace simpleecho_p2p
{
    public class Server
    {
        public ListenerHandler lh;

        public Server()
        {
            lh = new ServerDefaultListenerHandler(this);
        }

        public class ServerDefaultListenerHandler : DefaultListenerHandler
        {
            Server sm;
            ServerDeliveryService sds;

            public ServerDefaultListenerHandler(Server sm)
            {
                this.sm = sm;
                this.sds = new ServerDeliveryService(this);
            }

            protected override Etch.Transport.MessageHandler<MailboxMessageSource> NewMessageHandler()
            {
                return new ServerDeliveryService(this);
            }

            protected override ValueFactory NewValueFactory()
            {
                return new ValueFactorySimpleEcho();
            }

            public override string ToString()
            {
                return "ServerDefaultListenerHandler";
            }
            
            public class ServerDeliveryService : DefaultDeliveryService<StubSimpleEcho>
            {
                ServerDefaultListenerHandler sdlh;

                public ServerDeliveryService(ServerDefaultListenerHandler sdlh)
                {
                    this.sdlh = sdlh;
                }

                protected override StubSimpleEcho NewStub(MailboxMessageSource src)
                {
                    this.SetMailboxMessageSource(src);
                    RemoteSimpleEcho rsc = new RemoteSimpleEcho(this);                    
                    return new StubSimpleEcho(new SimpleEchoImpl(rsc), null, null);
                }

                public override string ToString()
                {
                    return "ServerDeliveryService";
                }
            }

        }
    }
}
