using System;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Support;
using Etch.Transport;

namespace etch.examples.benchmark.etch
{
    public class PerfListener
    {

        public static void Main( String[] args )
        {
            Console.WriteLine( "PerfListener" );

            Pool queuedPool = null;
            Pool freePool = null;

            // create the listener
            ListenerHandler lh = PerfHelper.newPerfListener( queuedPool, freePool );
            Listener l = new Listener( lh, 5, "127.0.0.1", 4001, 0 );
            l.Start();
        }
    }
}
