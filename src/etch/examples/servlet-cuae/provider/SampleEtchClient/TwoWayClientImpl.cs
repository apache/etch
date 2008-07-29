using System;
using System.Collections.Generic;
using System.Text;

using cisco.etch.examples;

namespace Test
{
    class TwoWayClientImpl : TwoWayClient
    {
        public void sendServletMessage(ConstsTwoWay.Selection selection)
        {
            Console.WriteLine("The selection: " + selection);
        }

        public void bogus()
        {
        }
    }
}
