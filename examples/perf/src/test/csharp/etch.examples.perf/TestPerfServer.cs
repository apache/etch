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
using NUnit.Framework;
using org.apache.etch.examples.perf.types.Perf;

namespace org.apache.etch.examples.perf
{

    /// <summary>
    /// Unit Tests to test Perf
    /// </summary>
    /// 
    [TestFixture]
    public class TestPerfServer
    {
        private static RemotePerfServer server;

        private static ServerFactory listener;

        [TestFixtureSetUp]
        public void StartListener()
        {
            String uri = "tcp://localhost:4010";

            MainPerfListener implFactory = new MainPerfListener();
            listener = PerfHelper.NewListener(uri, null, implFactory);

            listener.TransportControl(TransportConsts.START_AND_WAIT_UP, 4000);

            Console.WriteLine("Listener Started");

        }

        [SetUp]
        public void MakeConnection()
        {
            String uri = "tcp://localhost:4010";

            MainPerfClient client = new MainPerfClient();
            server = PerfHelper.NewServer(uri, null, client);

            server._StartAndWaitUp(4000);
        }

        [Test]
        public void CheckConnection()
        {
            Assert.IsNotNull(server);
        }

        [Test]
        public void TestAdd()
        {
            int? result = server.add(3, 5);
            Assert.AreEqual(8, result);
        }

        [Test]
        public void TestAdd2()
        {
            DateTime? d = DateTime.Now;
            long? ms = 1000;
            DateTime? expectedDate = d.Value + new TimeSpan(ms.Value * 10000);
            DateTime? retDate = server.add2(d, ms);
            Assert.AreEqual(expectedDate.Value.ToString(), retDate.Value.ToString());
        }

        [Test]
        public void TestSum()
        {
            int[] a = { 1, 2, 3 };
            int? result = server.sum(a);
            Assert.AreEqual(6, result);
        }

        [Test]
        public void TestDistance()
        {
            Point a = new Point(2, 3);
            Point b = new Point(4, 5);
            Point result = server.dist(a, b);
            Assert.AreEqual(2, result.x);
            Assert.AreEqual(2, result.y);
        }


        [TearDown]
        public void CloseConnection()
        {
            if (server != null)
            {
                server._Stop();
            }
        }

        [TestFixtureTearDown]
        public void StopListener()
        {
            if (listener != null)
            {
                listener.TransportControl(TransportConsts.STOP_AND_WAIT_DOWN, 4000);
                listener = null;
            }
        }

    }
}