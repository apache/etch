using System;
using System.Collections.Generic;
using System.Text;

using Etch.Transport;
using Etch.Msg;

using etch.examples.ccmuser;

namespace etch.examples.ccmuser.client
{
    class ClientMain
    {
        public ClientDeliveryService cds = null;
        public TcpConnection c = null;
        public DefaultDeliveryService<StubCcmUserClient> svc = null;
        public ValueFactoryCcmUser vf = null;
        public RemoteCcmUserServer rfs = null;
        public StubCcmUserClient sfc = null;

        public ClientMain()
        {
            vf = new ValueFactoryCcmUser();
            c = new TcpConnection(null, "127.0.0.1", 4021, 0);
            sfc = new StubCcmUserClient(new CcmUserClientImpl(), null, null);
            cds = new ClientDeliveryService(this);
            svc = DefaultDeliveryService<StubCcmUserClient>.SetUp(cds, c, vf);
            rfs = new RemoteCcmUserServer(svc);
        }

        public class ClientDeliveryService : DefaultDeliveryService<StubCcmUserClient>
        {
            ClientMain cm = null;

            public ClientDeliveryService(ClientMain cm)
            {
                this.cm = cm;
            }

            protected override StubCcmUserClient NewStub(MailboxMessageSource src)
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

            cm.rfs.login("10.89.31.49", "Admin", "metreos1");

            ConstsCcmUser.UserConfiguration userConfig = cm.rfs.getUserConfig("metreos");

            System.Console.Read();

            cm.svc.ShutdownOutput();
            cm.c.Stop();
            cm.c.WaitDown(4000);
        }
    }
}
