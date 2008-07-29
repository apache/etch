using System;
using System.Collections.Generic;
using System.Text;

namespace etch.examples.simpleecho
{
    class SimpleEchoClientImpl : SimpleEchoClient
    {
        public void onNewMessage(string msg)
        {
            System.Console.WriteLine(msg);
        }
    }
}
