using System;
using System.Collections.Generic;
using System.Text;

using Etch.Transport;
using Etch.Msg;

using cisco.etch.examples;

namespace Test
{
    class Program
    {
        static void Main(string[] args)
        {
            EtchServer sm = new EtchServer();
            Listener listener = new Listener(sm.lh, 5, "127.0.0.1", 4051, 0);
            listener.Start();
            listener.WaitUp(4000);
        }

        public class EtchServer
        {
            public MyDefaultListenerHandler lh;

            public EtchServer()
            {
                lh = new MyDefaultListenerHandler(this);
            }

            public class MyDefaultListenerHandler : DefaultListenerHandler
            {
                EtchServer sm;
                MyDeliveryService sds;

                public MyDefaultListenerHandler(EtchServer sm)
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
                    return new ValueFactoryTwoWay();
                }

                public override string ToString()
                {
                    return "TwoWayServer";
                }

                public class MyDeliveryService : DefaultDeliveryService<StubTwoWayServer>
                {
                    MyDefaultListenerHandler sdlh;

                    public MyDeliveryService(MyDefaultListenerHandler sdlh)
                    {
                        this.sdlh = sdlh;
                    }

                    protected override StubTwoWayServer NewStub(MailboxMessageSource src)
                    {
                        this.SetMailboxMessageSource(src);
                        RemoteTwoWayClient rsc = new RemoteTwoWayClient(this);
                        return new StubTwoWayServer(new SimpleImpl(rsc), null, null);
                    }

                    public override string ToString()
                    {
                        return "MyDeliveryService";
                    }
                }
            }
        }
    }
}
