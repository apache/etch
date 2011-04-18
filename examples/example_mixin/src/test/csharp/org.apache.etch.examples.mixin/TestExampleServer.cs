// $Id: TestHelloWorldServer.cs 743358 2009-02-11 15:18:15Z sccomer $
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

namespace org.apache.etch.examples.mixin
{

    /// <summary>
    /// Unit Tests to test HelloWorld
    /// </summary>
    /// 
    [TestFixture]
    public class TestExampleServer
    {
        private static RemoteExampleServer server;

        private static ServerFactory listener;

        [TestFixtureSetUp]
        public void StartListener()
        {
            String uri = "tcp://127.0.0.1:4010";

            MainExampleListener implFactory = new MainExampleListener();
            listener = ExampleHelper.NewListener(uri, null, implFactory);

            listener.TransportControl(TransportConsts.START_AND_WAIT_UP, 4000);

            Console.WriteLine("Listener Started");

        }

        [SetUp]
        public void MakeConnection()
        {
            String uri = "tcp://127.0.0.1:4010";

            MainExampleClient client = new MainExampleClient();
            server = ExampleHelper.NewServer(uri, null, client);

            server._StartAndWaitUp(4000);
        }

        [Test]
        public void CheckConnection()
        {
            Assert.IsNotNull(server);
        }

        [Test]
        public void TestSayHallo()
        {
	        string s = server.say_hello("Hello");
            Assert.AreEqual(s, "Server: Hello");
        }

        [Test]
        public void TestSayHalloMixin()
        {
            string s = server.say_hello_mixin("Hello");
            Assert.AreEqual(s, "Server-Mixin: Hello");
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
