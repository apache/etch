using System;
using System.Collections.Generic;
using System.Text;
using etch.examples.chat;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using System.Collections;
using System.Runtime.CompilerServices;

namespace etch.examples.chat
{
    public class TestRemoteChatListener
    {
        /// <summary>
        /// Program entry point
        /// </summary>
        /// <param name="args"></param>
        public static void Main2( String[] args )
        {
            Console.WriteLine( "TestRemoteChatListener" );

            TodoManager tm = new TodoManager( 99, 20, 5, 20, 60, 1 );
            tm.Start();

            Pool queuedPool = new QueuedPool( tm );
            Pool freePool = new FreePool();

            ListenerHandler lh = ChatHelper.newChatServerListener( queuedPool, freePool );
            Listener l = new Listener( lh, 5, "127.0.0.1", 4001, 0 );
            l.Start();
            l.WaitUp( 4000 );
        }
    }
}
