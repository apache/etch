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
using org.apache.etch.tests;

namespace etch.tests
{
    [TestFixture]
    public class TestAsyncClient
    {

        private static RemoteAsyncServer server;
        private static ServerFactory listener;

        [TestFixtureSetUp]
        public void Setup()
        {
      //      MainAsyncListener.Main(new String[] { });
  
            String uri = "tcp://localhost:4010";         

            MainAsyncListener implFactory = new MainAsyncListener();
            listener = AsyncHelper.NewListener(uri, null, implFactory);

            listener.TransportControl(TransportConsts.START_AND_WAIT_UP, 4000);

            MainAsyncClient client = new MainAsyncClient();
            server = AsyncHelper.NewServer(uri, null, client);
          

            server._StartAndWaitUp(4000);
        }


        [Test]
        public void sync1()
        {

            Assert.AreEqual(0, server.sync(0));

        }

       

        [Test]
        public void async_queued1()
        {
            for (int i = 0; i < 5; i++)
                Assert.AreEqual(i, server.async_queued(i));
        }
        
        
        

        
        [Test]
	    public void async_free() 
	    {
		    for (int i = 0; i < 10; i++)
                Assert.AreEqual(i, server.async_free(i));
	    }

        [TestFixtureTearDown]
        public void Method_CloseNewTestListener()
        {
           
                server._StopAndWaitDown(4000);
                listener.TransportControl(TransportConsts.STOP_AND_WAIT_DOWN,4000);
        }
 
    } 
}
