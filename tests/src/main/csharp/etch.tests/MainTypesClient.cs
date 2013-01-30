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
using System.Collections;
using Org.Apache.Etch.Bindings.Csharp.Util;
using org.apache.etch.tests;

namespace etch.bindings.csharp.examples.types
{

    ///
    ///<summary> Main implementation for TypesClient. </summary>
    ///
    public class MainTypesClient : TypesHelper.TypesClientFactory
    {
        ///
        /// <summary>Main for TypesClient</summary>
        /// <param name="args"></param>
        ///
        public static void Main(String[] args)
        {
            string uri = "tcp://localhost:4001";

            MainTypesListener.Main1(null);
            MainTypesClient implFactory = new MainTypesClient();
            RemoteTypesServer server = TypesHelper.NewServer(uri, null, implFactory);

            server._TransportControl(TransportConsts.START_AND_WAIT_UP, 4000);

            Hashtable table = new Hashtable();
            table.Add("1", "Entry1");
            table.Add("2", "Entry2");

            IDictionary map1 = server.test1(table);


            ArrayList list = new ArrayList();

            list.Add("List1");
            IList list1 = server.test3(list);



            DateTime? date = server.test4(DateTime.Now);


            Hashtable set = new Hashtable();
            set.Add("1", null);
            set.Add("2", null);


            Hashtable retset = server.test2(set);

            server._TransportControl(TransportConsts.STOP_AND_WAIT_DOWN, 4000);

        }

        ///
        /// <summary>Constructs the MainTypesClient.</summary>
        ///
        public MainTypesClient()
        {
            // Nothing to do.
        }

        ///
        /// <summary>Return a new instance of TypesClient.</summary>
        /// @param server
        ///
        public TypesClient NewTypesClient(RemoteTypesServer server)
        {
            return new ImplTypesClient(server);
        }

    }

}