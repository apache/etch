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
using org.apache.etch.tests;

/**
 * Call to message translator for ClosingClient.
 */
namespace etch.tests
{

	public class ImplClosingClient : BaseClosingClient, ObjSession
	{

		/// 
		/// <summary>Constructs the ImplClosingClient.</summary>
		///
 		/// <param name=server></param>
 		///
		public ImplClosingClient(RemoteClosingServer server )
		{
		    this.server = server;
		}

	    private readonly RemoteClosingServer server;

      

        public new void _SessionNotify(Object eventObj)
        {
            Console.WriteLine("ImplClosingClient._sessionNotify" + eventObj);
        }

		
	}
}