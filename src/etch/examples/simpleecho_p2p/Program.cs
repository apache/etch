using System;
using System.Collections.Generic;
using System.Text;

using Etch.Transport;

namespace simpleecho_p2p
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length < 4)
            {
                System.Console.WriteLine("Usage: simpleecho_p2p my_ip my_port peer_ip peer_port");
                return;
            }

            // run server first
            Server svr = new Server();
            Listener l = new Listener(svr.lh, 5, args[0], System.Convert.ToInt32(args[1]), 0);
            l.Start();
            l.WaitUp(4000);

            System.Console.WriteLine("Hit enter to send message...");
            System.Console.ReadLine();

            // set up client
            Client client = new Client(args[2], System.Convert.ToInt32(args[3]));

            client.c.Start();
            client.c.WaitUp(4000);

            do
            {
                client.rfs.write("hi!!");
            }
            while (System.Console.ReadKey().KeyChar != 'q');

            client.svc.ShutdownOutput();
            client.c.Stop();
            client.c.WaitDown(4000);

            l.ShutdownInput();
            l.ShutdownOutput();
            l.Stop();
            l.WaitDown(4000);

        }
    }
}
