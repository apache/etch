// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;
using org.apache.etch.examples.perf.types.Perf;

namespace org.apache.etch.examples.perf
{
    ///<summary>Main implementation for PerfClient.</summary>
    public class MainPerfClient : PerfHelper.PerfClientFactory, PerfHelper.PerfServerFactory
    {
        ///<summary>Main for PerfClient</summary>
        ///<param name="args">Command Line Arguments</param>
        public static void Main(String[] args)
        {
            const string uri = "tcp://localhost:4004";
            bool startListener = false;
            const int maxWaitTime = 4000;

            ServerFactory listener;

            if (startListener)
            {
                listener = PerfHelper.NewListener(uri, null, new MainPerfClient());
                listener.TransportControl(TransportConsts.START_AND_WAIT_UP, maxWaitTime);
            }
            else
            {
                listener = null;
            }

            MainPerfClient implFactory = new MainPerfClient();

            // run a preliminary test

            {
                RemotePerfServer server = PerfHelper.NewServer(uri, null, implFactory);

                // Connect to the service
                server._StartAndWaitUp(maxWaitTime);

                Assertion.Check(server.add(2, 3) == 5, "server.add(2, 3) == 5");

                Assertion.Check(server.add(3, 4) == 7, "server.add( 3, 4 ) == 7");

                Assertion.Check(server.sum(new int[] {1, 2, 3}) == 6,
                                "server.sum( new int[] { 1, 2, 3 } ) == 6");

                Assertion.Check(server.sum(new int[] {2, 3, 4}) == 9,
                                "server.sum( new int[] { 2, 3, 4 } ) == 9");

                server.report(18, "starting");

                server.report(19, "stopping");

                Point r = server.dist(new Point(1, 2), new Point(3, 5));
                Assertion.Check(r.x == 2, "r.x == 2");
                Assertion.Check(r.y == 3, "r.y == 3");

                r = server.dist(new Point(10, 9), new Point(4, 5));
                Assertion.Check(r.x == -6, "r.x == -6");
                Assertion.Check(r.y == -4, "r.y == -4");

                DateTime t0 = DateTime.Now;
                long adj = 3600000L;

                DateTime? t1 = server.add2(t0, adj);
                TimeSpan ts = t1.Value - t0;
                //Console.WriteLine("ts = {0}", ts.Ticks);
                Assertion.Check(IsNearly(ts.Ticks / 10000, adj, 1),
                    String.Format("server.add2( {0}, {1} ) => {2} [bogus!]", t0, adj, t1));

                t1 = server.add2(t0, -adj);
                ts = t1.Value - t0;
                //Console.WriteLine("ts = {0}", ts.Ticks);
                Assertion.Check(IsNearly(ts.Ticks / 10000, -adj, 1),
                    String.Format("server.add2( {0}, {1} ) => {2} [bogus!]", t0, -adj, t1));

                server.report2(DateTime.Now, 20, "begin");

                server.report2(DateTime.Now, 21, "end");

                // Disconnect from the service
                server._StopAndWaitDown(maxWaitTime);
            }

            const int runtime = 60;
            const int trials = 3;
            //const bool full = false;

			new PerfTest( "add", runtime, trials,
                delegate(long n)
			    {
					    RemotePerfServer server = PerfHelper.NewServer( uri, null, implFactory );
					    server._StartAndWaitUp( maxWaitTime );
    					
					    while (n-- > 0)
						    server.add( 1000000000, 2000000000 );
    					
					    server._StopAndWaitDown( maxWaitTime );
			    }).Run();

            if (listener != null)
                listener.TransportControl(TransportConsts.STOP_AND_WAIT_DOWN, maxWaitTime);

            Console.WriteLine("done");
        }

        private static bool IsNearly(long a, long b, long diff)
        {
            return Math.Abs(a - b) <= diff;
        }

        ///<summary>Return a new instance of PerfClient.</summary>
        ///<param name="server">Reference to remote service</param>
        ///<returns>Client Implementation</returns>
        public PerfClient NewPerfClient(RemotePerfServer server)
        {
            return new ImplPerfClient(server);
        }

        public PerfServer NewPerfServer(RemotePerfClient client)
        {
            return new ImplPerfServer(client);
        }
    }
}