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
using System.Collections.Generic;
using org.apache.etch.examples.distmap.types.DistributedHashTable;

namespace org.apache.etch.examples.distmap
{
    ///<summary>Implementation for ImplDistributedHashTableServer</summary>
    public class ImplDistributedHashTableServer : BaseDistributedHashTableServer
    {
        /// <summary>Constructs the ImplDistributedHashTableServer.</summary>
        /// <param name="client">a connection to the client session. Use this to
        /// send a message to the client.</param>
        /// <param name="map"></param>
        public ImplDistributedHashTableServer(RemoteDistributedHashTableClient client, IDictionary<string, object> map)
        {
            this.map = map;
        }

        private IDictionary<string, object> map;


        /// <summary>
        /// Gets all elements in DHT
        /// </summary>
        /// <returns>An array of entries</returns>
        public override Entry[] getAll()
        {
            lock (map)
            {
                Entry[] list = new Entry[map.Count];
                int index = 0;
                foreach (string str in map.Keys)
                {
                    list[index++] = new Entry(str, map[str]);
                }
                return list;
            }
        }
        /// <summary>
        /// Gets an object from DHT
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public override object getObject(string key)
        {
            object value;
            if (!map.TryGetValue(key, out value))
            {
                Console.WriteLine(" {0} not found in DHT ", key);
            }

            return value;
        }

        /// <summary>
        /// Puts object in Map
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        public override object putObject(string key, object value)
        {
            map.Add(key, value);
            return value;
        }


        /// <summary>
        /// Removes Object from DHT
        /// </summary>
        /// <param name="key"></param>
        /// <returns></returns>
        public override object removeObject(string key)
        {
            map.Remove(key);
            return null;
        }
    
        /// <summary>
        /// Gets Size of DHT
        /// </summary>
        /// <returns></returns>
        public override int? size()
        {
            return map.Count;
        }
    }
}