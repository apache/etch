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
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace org.apache.etch.examples.distmap
{
	///<summary>Main implementation for DistributedHashTableListener.</summary>
	public class MainDistributedHashTableListener : DistributedHashTableHelper.DistributedHashTableServerFactory
	{
 		///<summary>Main for DistributedHashTableListener.</summary>
		///<param name="args">Command Line Arguments</param>
		public static void Main(String[] args)
		{
			// TODO: Change to correct URI
			string uri = "tcp://localhost:4007";

            ServerFactory listener = DistributedHashTableHelper.NewListener(uri, null, new MainDistributedHashTableListener());
			
			// Start the Listener
			listener.TransportControl( TransportConsts.START_AND_WAIT_UP, 4000 );

            Console.WriteLine("Listener Started !!!");
		}

        private IDictionary<string ,object > map = new Dictionary<string, object>();

		///<summary>Return a new instance of DistributedHashTableServer.</summary>
		///<param name="client">Reference to remote service</param>
		///<returns>Server Implementation</returns>
		public DistributedHashTableServer NewDistributedHashTableServer( RemoteDistributedHashTableClient client )
		{
			return new ImplDistributedHashTableServer( client,map );
		}
	}
}