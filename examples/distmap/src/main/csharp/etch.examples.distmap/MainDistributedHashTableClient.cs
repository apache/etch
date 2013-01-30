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
using org.apache.etch.examples.distmap.types.DistributedHashTable;

namespace org.apache.etch.examples.distmap
{
    ///<summary>Main implementation for DistributedHashTableClient.</summary>
    public class MainDistributedHashTableClient : DistributedHashTableHelper.DistributedHashTableClientFactory
    {
        ///<summary>Main for DistributedHashTableClient</summary>
        ///<param name="args">Command Line Arguments</param>
        public static void Main(String[] args)
        {
            // TODO: Change to correct URI
            string uri = "tcp://localhost:4007";

            RemoteDistributedHashTableServer server = DistributedHashTableHelper.NewServer(uri, null, new MainDistributedHashTableClient());

            // Connect to the service
            server._StartAndWaitUp(4000);

            const bool myBool1 = true;
            const sbyte myByte1 = 1;
            const short myShort1 = 234;
            const int myInt1 = 345345;
            const long myLong1 = 456456456456L;
            const float myFloat1 = 1.234f;
            const double myDouble1 = 2.345;
            const string myString1 = "abc";

            object myObj;

            Console.WriteLine("DHT sixe is " + server.size());

            // getAll - DHT is empty
            foreach (Entry entry in server.getAll())
                Console.WriteLine("getAll: {0},{1} ", entry.key, entry.value);

            // putObject for each constant type
            server.putObject("boolean1", myBool1);
            Console.WriteLine("Put {0} in DHT", myBool1);

            server.putObject("byte1", myByte1);
            Console.WriteLine("Put {0} in DHT", myByte1);


            server.putObject("short1", myShort1);
            Console.WriteLine("Put {0} in DHT", myShort1);

            server.putObject("int1", myInt1);
            Console.WriteLine("Put {0} in DHT", myInt1);

            server.putObject("long1", myLong1);
            Console.WriteLine("Put {0} in DHT", myLong1);

            server.putObject("float1", myFloat1);
            Console.WriteLine("Put {0} in DHT", myFloat1);

            server.putObject("double1", myDouble1);
            Console.WriteLine("Put {0} in DHT", myDouble1);

            server.putObject("string1", myString1);
            Console.WriteLine("Put {0} in DHT", myString1);

            // size - DHT has entries
            Console.WriteLine("DHT Size is {0} ", server.size());

            // getAll - DHT has entries
            foreach (Entry entry in server.getAll())
                Console.WriteLine("getAll: {0} {1} ", entry.key, entry.value);

            // getObject for each constant type
            myObj = server.getObject("boolean1");
            Console.WriteLine("Value of myBool1 is {0} ", myObj);

            myObj = server.getObject("byte1");
            Console.WriteLine("Value of myByte1 is {0} ", myObj);

            myObj = server.getObject("short1");
            Console.WriteLine("Value of myShort1 is {0} ", myObj);

            myObj = server.getObject("int1");
            Console.WriteLine("Value of myInt1 is {0} ", myObj);

            myObj = server.getObject("long1");
            Console.WriteLine("Value of myLong1 is {0} ", myObj);

            myObj = server.getObject("float1");
            Console.WriteLine("Value of myFloat1 is {0} ", myObj);

            myObj = server.getObject("double1");
            Console.WriteLine("Value of myDouble1 is {0} ", myObj);

            myObj = server.getObject("string1");
            Console.WriteLine("Value of myString1 is {0} ", myObj);

            // removeObject for each constant type
            server.removeObject("boolean1");
            Console.WriteLine("removing Object boolean1 ");

            server.removeObject("byte1");
            Console.WriteLine("removing Object byte1 ");

            server.removeObject("short1");
            Console.WriteLine("removing Object short1 ");

            server.removeObject("int1");
            Console.WriteLine("removing Object int1 ");

            server.removeObject("long1");
            Console.WriteLine("removing Object long1 ");

            server.removeObject("float1");
            Console.WriteLine("removing Object float1 ");

            server.removeObject("double1");
            Console.WriteLine("removing Object double1 ");

            server.removeObject("string1");
            Console.WriteLine("removing Object string1 ");

            // size - Verify that DHT is empty after removing all items
            Console.WriteLine("DHT Size is {0}", server.size());

            // getAll - Verify that DHT is empty after removing all items
            foreach (Entry entry in server.getAll())
                Console.WriteLine("getAll: {0} {1} ", entry.key, entry.value);

            Console.WriteLine("Have a nice day!");

            // Disconnect from the service
            server._StopAndWaitDown(4000);
        }

        ///<summary>Return a new instance of DistributedHashTableClient.</summary>
        ///<param name="server">Reference to remote service</param>
        ///<returns>Client Implementation</returns>
        public DistributedHashTableClient NewDistributedHashTableClient(RemoteDistributedHashTableServer server)
        {
            return new ImplDistributedHashTableClient(server);
        }
    }
}