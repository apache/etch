using System;
using System.Collections.Generic;
using System.Text;

using Etch.Transport;
using Etch.Msg;

namespace etch.examples.simpleecho
{
    class ClientMain
    {
        public ClientDeliveryService cds = null;
        public TcpConnection c = null;
        public DefaultDeliveryService<StubSimpleEchoClient> svc = null;
        public ValueFactorySimpleEcho vf = null;
        public RemoteSimpleEchoServer rfs = null;
        public StubSimpleEchoClient sfc = null;

        public ClientMain()
        {
            vf = new ValueFactorySimpleEcho();
            c = new TcpConnection(null, "127.0.0.1", 4001, 0);
            sfc = new StubSimpleEchoClient(new SimpleEchoClientImpl(), null, null);
            cds = new ClientDeliveryService(this);
            svc = DefaultDeliveryService<StubSimpleEchoClient>.SetUp(cds, c, vf);
            rfs = new RemoteSimpleEchoServer(svc);
        }

        public class ClientDeliveryService : DefaultDeliveryService<StubSimpleEchoClient>
        {
            ClientMain cm = null;

            public ClientDeliveryService(ClientMain cm)
            {
                this.cm = cm;
            }

            protected override StubSimpleEchoClient NewStub(MailboxMessageSource src)
            {
                return cm.sfc;
            }

            public override string ToString()
            {
                return "ClientDeliveryService";
            }
        }

        static void Main(string[] args)
        {
            ClientMain cm = new ClientMain();

            cm.c.Start();
            cm.c.WaitUp(4000);

            cm.rfs.write("hi!!");
            System.Console.Read();

            cm.svc.ShutdownOutput();
            cm.c.Stop();
            cm.c.WaitDown(4000);
        }
    }
}
