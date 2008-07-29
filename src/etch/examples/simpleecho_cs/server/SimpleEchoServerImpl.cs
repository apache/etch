using System;
using System.Collections.Generic;
using System.Text;

namespace etch.examples.simpleecho
{
    public class SimpleEchoServerImpl : SimpleEchoServer
    {
        public SimpleEchoClient client;

        public SimpleEchoServerImpl(SimpleEchoClient client)
        {
            this.client = client;
        }

        public void write(string msg)
        {
            client.onNewMessage("You said: " + msg);
        }
    }
}
