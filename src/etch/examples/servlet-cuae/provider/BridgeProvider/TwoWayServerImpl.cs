using System;
using System.Collections.Generic;
using System.Text;

using cisco.etch.examples;

namespace Cisco.Providers.TwoWay
{
    public class TwoWayServerImpl : TwoWayServer
    {
        public TwoWayClient client;

        protected TwoWayProvider provider;

        // this is called everytime a client connects.
        public TwoWayServerImpl(TwoWayProvider provider, TwoWayClient client)
        {
            this.provider = provider;
            this.client = client;

            provider.LogWrite(System.Diagnostics.TraceLevel.Info, "Received a connection from the client.");
        }

        public void sendCUAEMessage(string msg)
        {
           // invoke provider callback method
            provider.LogWrite(System.Diagnostics.TraceLevel.Info, "Received a message from the servlet!\nMessage: " + msg);

            provider.PushMessage(msg);
        }
    }
}
