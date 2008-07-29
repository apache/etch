using System;
using System.Collections.Generic;
using System.Text;

using Etch.Transport;
using Etch.Msg;

namespace etch.examples.simpleecho
{
    class ServerMain
    {
        public ListenerHandler lh;

        public ServerMain()
        {
            lh = new MyDefaultListenerHandler(this);
        }

        public class MyDefaultListenerHandler : DefaultListenerHandler
        {
            ServerMain sm;
            MyDeliveryService sds;

            public MyDefaultListenerHandler(ServerMain sm)
            {
                this.sm = sm;
                this.sds = new MyDeliveryService(this);
            }

            protected override Etch.Transport.MessageHandler<MailboxMessageSource> NewMessageHandler()
            {
                return new MyDeliveryService(this);
            }

            protected override ValueFactory NewValueFactory()
            {
                return new ValueFactorySimpleEcho();
            }

            public override string ToString()
            {
                return "MyDefaultListenerHandler";
            }
            
            public class MyDeliveryService : DefaultDeliveryService<StubSimpleEchoServer>
            {
                MyDefaultListenerHandler sdlh;

                public MyDeliveryService(MyDefaultListenerHandler sdlh)
                {
                    this.sdlh = sdlh;
                }

                protected override StubSimpleEchoServer NewStub(MailboxMessageSource src)
                {
                    this.SetMailboxMessageSource(src);
                    RemoteSimpleEchoClient rsc = new RemoteSimpleEchoClient(this);
                    return new StubSimpleEchoServer(new SimpleEchoServerImpl(rsc), null, null);
                }

                public override string ToString()
                {
                    return "MyDeliveryService";
                }
            }

        }

        public static void Main(String[] args)
        {
            ServerMain sm = new ServerMain();
            Listener l = new Listener(sm.lh, 5, "127.0.0.1", 4001, 0);
            l.Start();
            l.WaitUp(4000);
        }
    }
}
