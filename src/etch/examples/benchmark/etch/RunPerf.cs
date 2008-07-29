using System;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using Etch.Transport.Util;
using System.Diagnostics;

namespace etch.examples.benchmark.etch
{
    public class RunPerf
    {
        private static PerfServer _server;
        private static int[] _values;

        public static void Main( String[] args )
        {
            Console.WriteLine( "RunPerf" );

            Pool queuedPool = null;
            Pool freePool = null;

            // connect to server
            TcpConnection c = new TcpConnection( null, "127.0.0.1", 4001, 0 );
            _server = PerfHelper.newPerfServer( c, new MyPerfClient(), queuedPool, freePool );

            c.Start();
            c.WaitUp( 4000 );

            // make sure server is working

            Debug.Assert( _server.add( 4, 5 ) == 9, "server.add( 4, 5 ) == 9" );

            Debug.Assert( _server.add( 2, 3 ) == 5, "server.add( 2, 3 ) == 5" );

            Debug.Assert( _server.sum( new int[] { 1, 2, 3 } ) == 6, "server.sum( new int[] {1, 2, 3} ) == 6" );

            Debug.Assert( _server.sum( new int[] { 2, 4, 6 } ) == 12, "server.sum( new int[] { 2, 4, 6 } ) == 12" );

            _server.report( 23, "all working" );
            _server.report( 19, "stopping" );

            PerfTest pt1 = new PerfTest( "add", 10, 5, RunAdd );
            pt1.Run();

            _values = new int[ 1000 ];
            for ( int i = 0; i < _values.Length; i++ )
                _values[ i ] = i;

            PerfTest pt2 = new PerfTest( "sum", 10, 5, RunSum );
            pt2.Run();

            PerfTest pt3 = new PerfTest( "report", 10, 5, RunReport );
            pt3.Run();

            ( ( RemotePerfServer ) _server )._ShutdownOutput();
            c.WaitDown( 4000 );
        }

        #region Run Delegates

        public static void RunAdd( long n )
        {
            while ( n-- > 0 )
                _server.add( 1000000000, 2000000000 );
        }

        public static void RunSum( long n )
        {
            while ( n-- > 0 )
                _server.sum( _values );
        }

        public static void RunReport( long n )
        {
            while ( n-- > 0 )
                _server.report( 23,
                    // 50 characters:
                    "this message describes the specifics of a code 23." );
        }

        #endregion

        public class MyPerfClient : PerfClient
        {
            public MyPerfClient()
                : base()
            { }
        }
    }
}
