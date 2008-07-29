using System;
using System.Collections.Generic;
using System.Text;

using cisco.etch.examples;

namespace Test
{
    public class SimpleImpl : TwoWayServer
    {
        protected TwoWayClient client;

        // this is called everytime a client connects.
        public SimpleImpl(TwoWayClient client)
        {
            this.client = client;

            Console.WriteLine("Received a connection from the client.");
        }

        public void sendCUAEMessage(string msg)
        {
            // invoke provider callback method
            Console.WriteLine("Received a message from the servlet!\nMessage: " + msg);

        }
    }
}
