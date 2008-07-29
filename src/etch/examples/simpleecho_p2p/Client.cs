using System;
using System.Collections.Generic;
using System.Text;

using Etch.Transport;
using Etch.Msg;

using etch.examples.simpleecho;

namespace simpleecho_p2p
{
    class Client
    {
        public ClientDeliveryService cds = null;
        public TcpConnection c = null;
        public DefaultDeliveryService<StubSimpleEcho> svc = null;
        public ValueFactorySimpleEcho vf = null;
        public RemoteSimpleEcho rfs = null;
        public StubSimpleEcho sfc = null;

        public Client(string serverIp, int serverPort)
        {
            vf = new ValueFactorySimpleEcho();
            c = new TcpConnection(null, serverIp, serverPort, 0);
            sfc = new StubSimpleEcho(new SimpleEchoImpl(), null, null);
            cds = new ClientDeliveryService(this);
            svc = DefaultDeliveryService<StubSimpleEcho>.SetUp(cds, c, vf);
            rfs = new RemoteSimpleEcho(svc);
        }

        public class ClientDeliveryService : DefaultDeliveryService<StubSimpleEcho>
        {
            Client cm = null;

            public ClientDeliveryService(Client cm)
            {
                this.cm = cm;
            }

            protected override StubSimpleEcho NewStub(MailboxMessageSource src)
            {
                return cm.sfc;
            }

            public override string ToString()
            {
                return "ClientDeliveryService";
            }
        }

    }
}
