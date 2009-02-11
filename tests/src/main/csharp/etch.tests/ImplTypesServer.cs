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
using org.apache.etch.tests;

/**
 * Call to message translator for TypesServer.
 */
namespace etch.bindings.csharp.examples.types
{

    public class ImplTypesServer : BaseTypesServer
    {

        /// 
        /// <summary>Constructs the ImplTypesServer.</summary>
        ///
        /// <param name=client></param>
        ///
        public ImplTypesServer(RemoteTypesClient client)
        {
            this.client = client;
            _delegate_test1 = TestMap;
            _delegate_test2 = TestSet;
            _delegate_test3 = TestList;
            _delegate_test4 = TestDateTime;
        }

        private readonly RemoteTypesClient client;

        // Implement delegates or provide implementation of client

        public System.Collections.IDictionary TestMap(System.Collections.IDictionary map)
        {
            Console.WriteLine("map = " + map);
            foreach (object key in map.Keys)
            {
                string value = (string)map[key];
                Console.WriteLine(" The key is {0} and value is {1}", key, value);
            }
            return map;
        }

        public System.Collections.IList TestList(System.Collections.IList list)
        {
            Console.WriteLine("List = " + list);
            foreach (object key in list)
            {
                Console.WriteLine(" The value in list is {0}", key);
            }
            return list;
        }

        public System.DateTime? TestDateTime(System.DateTime? date)
        {
            Console.WriteLine("DateTime = " + date);
            return date;
        }

        public System.Collections.Hashtable TestSet(System.Collections.Hashtable table)
        {
            foreach (object key in table.Keys)
            {
                Console.WriteLine(" The value in set is {0}", key);
            }
            return table;
        }

    }
}