using System;
using System.Collections.Generic;
using System.Text;

using Etch.Transport;
using Etch.Msg;

using etch.examples.ccmuser;

namespace etch.examples.ccmuser.server
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
                return new ValueFactoryCcmUser();
            }

            public override string ToString()
            {
                return "MyDefaultListenerHandler";
            }
            
            public class MyDeliveryService : DefaultDeliveryService<StubCcmUserServer>
            {
                MyDefaultListenerHandler sdlh;

                public MyDeliveryService(MyDefaultListenerHandler sdlh)
                {
                    this.sdlh = sdlh;
                }

                protected override StubCcmUserServer NewStub(MailboxMessageSource src)
                {
                    this.SetMailboxMessageSource(src);
                    RemoteCcmUserClient rsc = new RemoteCcmUserClient(this);
                    return new StubCcmUserServer(new CcmUserServerImpl(rsc), null, null);
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
            Listener l = new Listener(sm.lh, 5, "127.0.0.1", 4021, 0);
            l.Start();
            l.WaitUp(4000);
        }
    }
}
