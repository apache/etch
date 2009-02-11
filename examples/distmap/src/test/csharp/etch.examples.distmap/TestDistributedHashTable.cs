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
using org.apache.etch.examples.distmap.types.DistributedHashTable;

namespace org.apache.etch.examples.distmap
{

    /// <summary>
    /// Unit Tests to test DistributedHashTable
    /// </summary>
    [TestFixture]
    public class TestDistributedHashTable
    {
        private static RemoteDistributedHashTableServer server;

        private static ServerFactory listener;

        public const bool myBool1 = true;
        public const sbyte myByte1 = 127;
        public const short myShort1 = 32767;
        public const int myInt1 = 2147483647;
        public const long myLong1 = 9223372036854775807L;
        public const float myFloat1 = 3.4028235e38f;
        public const double myDouble1 = 1.7976931348623157e308d;
        public const string myString1 = "abc";

        [TestFixtureSetUp]
        public void StartListener()
        {
            String uri = "tcp://localhost:4010";

            MainDistributedHashTableListener implFactory = new MainDistributedHashTableListener();
            listener = DistributedHashTableHelper.NewListener(uri, null, implFactory);

            listener.TransportControl(TransportConsts.START_AND_WAIT_UP, 4000);

            Console.WriteLine("Listener Started");

        }

        [SetUp]
        public void MakeConnection()
        {
            String uri = "tcp://localhost:4010";

            MainDistributedHashTableClient client = new MainDistributedHashTableClient();
            server = DistributedHashTableHelper.NewServer(uri, null, client);

            server._StartAndWaitUp(4000);
        }

        [ Test ]
        public void TestBoolean()
        {
            server.putObject("boolean1",myBool1);
            Object myObj = server.getObject("boolean1");
            Assert.AreEqual(myBool1,myObj);

            server.removeObject("boolean1");
            myObj = server.getObject("boolean1");
            Assert.IsNull(myObj);
        }

        [Test]
        public void TestByte()
        {
            server.putObject("byte1", myByte1);
            Object myObj = server.getObject("byte1");
            Assert.AreEqual(myByte1, myObj);

            server.removeObject("byte1");
            myObj = server.getObject("byte1");
            Assert.IsNull(myObj);
        }

        [Test]
        public void TestShort()
        {
            server.putObject("short1", myShort1);
            Object myObj = server.getObject("short1");
            Assert.AreEqual(myShort1, myObj);

            server.removeObject("short1");
            myObj = server.getObject("short1");
            Assert.IsNull(myObj);
        }

        [Test]
        public void TestInt()
        {
            server.putObject("int1", myInt1);
            Object myObj = server.getObject("int1");
            Assert.AreEqual(myInt1, myObj);

            server.removeObject("int1");
            myObj = server.getObject("int1");
            Assert.IsNull(myObj);
        }


        [Test]
        public void TestLong()
        {
            server.putObject("long1", myLong1);
            Object myObj = server.getObject("long1");
            Assert.AreEqual(myLong1, myObj);

            server.removeObject("long1");
            myObj = server.getObject("long1");
            Assert.IsNull(myObj);
        }

        [Test]
        public void TestFloat()
        {
            server.putObject("float1", myFloat1);
            Object myObj = server.getObject("float1");
            Assert.AreEqual(myFloat1, myObj);

            server.removeObject("float1");
            myObj = server.getObject("float1");
            Assert.IsNull(myObj);
        }

        [Test]
        public void TestDouble()
        {
            server.putObject("double1", myDouble1);
            Object myObj = server.getObject("double1");
            Assert.AreEqual(myDouble1, myObj);

            server.removeObject("double1");
            myObj = server.getObject("double1");
            Assert.IsNull(myObj);
        }

        [Test]
        public void TestString()
        {
            server.putObject("string1", myString1);
            Object myObj = server.getObject("string1");
            Assert.AreEqual(myString1, myObj);

            server.removeObject("string1");
            myObj = server.getObject("string1");
            Assert.IsNull(myObj);
        }

        [Test]
        public void TestSize()
        {
            server.putObject("int1", myInt1);
            server.putObject("string1", "abc");

            int? size = server.size();
            Assert.AreEqual(2,size);

            server.removeObject("int1");
            server.removeObject("string1");

            size = server.size();
            Assert.AreEqual(0, size);
        }

        [Test]
        public void TestGetAll()
        {
            int? size = server.size();
            Assert.AreEqual(0,size);

            server.putObject("int1", myInt1);
            server.putObject("string1", "abc");

            int count = 0;
            foreach (Entry entry in server.getAll())
            {
                if (entry.key != null && entry.value != null)
                    count++;
            }

            Assert.AreEqual(2,count);

            server.removeObject("int1");
            server.removeObject("string1");

            size = server.size();
            Assert.AreEqual(0,size);
            
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
                listener.TransportControl(TransportConsts.STOP_AND_WAIT_DOWN,4000);
                listener = null;
            }
        }
    
    }
}