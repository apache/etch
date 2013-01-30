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
using Org.Apache.Etch.Bindings.Csharp.Transport;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;
using org.apache.etch.tests;

namespace etch.tests
{
    [TestFixture]
    public class TestTest1Helper
    {
        public static ServerFactory listener;
        
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestHelper" );
        }

        [SetUp]
        public void Method_OpenNewTestListener()
        {
            TransportFactory.Define("blah", new TcpTransportFactory(false));

            string uri = "blah://localhost:4040";

            MainTest1Listener implFactory = new MainTest1Listener();

            listener = Test1Helper.NewListener( uri, null, implFactory );
            listener.TransportControl( TransportConsts.START_AND_WAIT_UP, 4000 );
        }

        [Test]
        public void Method_NewTestServer()
        {
            Boolean flag = true;

            string uri = "blah://localhost:4040";

            MainTest1Client client = new MainTest1Client();

            RemoteTest1Server server;

            try
            {
                server = Test1Helper.NewServer( uri, null, client );
                server._StartAndWaitUp( 4000 );

                // Insert your code here
                // -----------------------------------------------------------------------

                server._Stop();
            }
            catch ( Exception e )
            {
                flag = false;
                Console.WriteLine( e );
                Assert.IsTrue(flag);
            }

            Assert.IsTrue( flag );
        }

        [TearDown]
        public void Method_CloseNewTestListener()
        {
            listener.TransportControl( TransportConsts.STOP_AND_WAIT_DOWN, 0 );
        }
    }
}
