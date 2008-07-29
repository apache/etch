using System;
using System.Collections.Generic;
using System.Text;

using etch.examples.simpleecho;

namespace simpleecho_p2p
{
    class SimpleEchoImpl : SimpleEcho
    {
        public SimpleEcho peer;

        public SimpleEchoImpl()
        {
        }

        public SimpleEchoImpl(SimpleEcho peer)
        {
            this.peer = peer;
        }

        public void write(string msg)
        {
            System.Console.WriteLine("got it!");
            peer.onNewMessage("You said: " + msg);
        }

        public void onNewMessage(string msg)
        {
            System.Console.WriteLine(msg);
        }
    }
}
