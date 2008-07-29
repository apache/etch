using System;
using System.Collections.Generic;
using System.Text;

using Etch.Transport;
using Etch.Msg;

using cisco.etch.examples;

namespace Test
{
    class ClientMain
    {
        public ClientDeliveryService cds = null;
        public TcpConnection c = null;
        public DefaultDeliveryService<StubTwoWayClient> svc = null;
        public ValueFactoryTwoWay vf = null;
        public RemoteTwoWayServer rfs = null;
        public StubTwoWayClient sfc = null;

        public ClientMain()
        {
            vf = new ValueFactoryTwoWay();
            c = new TcpConnection(null, "10.89.31.162", 4051, 0);
            sfc = new StubTwoWayClient(new TwoWayClientImpl(), null, null);
            cds = new ClientDeliveryService(this);
            svc = DefaultDeliveryService<StubTwoWayClient>.SetUp(cds, c, vf);
            rfs = new RemoteTwoWayServer(svc);
        }

        public class ClientDeliveryService : DefaultDeliveryService<StubTwoWayClient>
        {
            ClientMain cm = null;

            public ClientDeliveryService(ClientMain cm)
            {
                this.cm = cm;
            }

            protected override StubTwoWayClient NewStub(MailboxMessageSource src)
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

            try
            {
                cm.rfs.sendCUAEMessage("hi");
            }
            catch(Exception e)
            {
                Console.WriteLine(e);
            }

            try
            {
                cm.rfs.sendCUAEMessage("hi");
            }
            catch(Exception e)
            {
                Console.WriteLine(e);
            }


            System.Console.Read();

            cm.svc.ShutdownOutput();
            cm.c.Stop();
            cm.c.WaitDown(4000);
        }
    }
}
