using System;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Support;
using Etch.Transport;

namespace etch.examples.distmap
{
    public class DistributedHashTableListener
    {
        public static void Main( String[] args )
        {
            Console.WriteLine( " DistributedHashTableListener " );

            TodoManager tm = new TodoManager( 99, 20, 5, 20, 60, 1 );
            tm.Start();

            Pool queuedPool = new QueuedPool( tm );
            Pool freePool = new FreePool();

            ListenerHandler lh = DistributedHashTableHelper.newDistributedHashTableListener( queuedPool, freePool );
            Listener l = new Listener( lh, 5, "127.0.0.1", 4001, 0 );
            l.Start();
        }
    }
}
