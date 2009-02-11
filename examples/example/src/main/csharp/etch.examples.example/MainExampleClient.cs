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

namespace org.apache.etch.examples.example
{
	///<summary>Main implementation for ExampleClient.</summary>
	public class MainExampleClient : ExampleHelper.ExampleClientFactory
	{
		///<summary>Main for ExampleClient</summary>
		///<param name="args">Command Line Arguments</param>
		public static void Main(String[] args)
		{
			// TODO: Change to correct URI
			string uri = "tcp://localhost:4002";
			
			RemoteExampleServer server = ExampleHelper.NewServer( uri, null, new MainExampleClient());
		
			// Connect to the service
			server._StartAndWaitUp( 4000 );

			// TODO: Insert Your Code Here

			// Disconnect from the service
			server._StopAndWaitDown( 4000 );			
		}

		///<summary>Return a new instance of ExampleClient.</summary>
		///<param name="server">Reference to remote service</param>
		///<returns>Client Implementation</returns>
		public ExampleClient NewExampleClient( RemoteExampleServer server )
		{
			return new ImplExampleClient( server );
		}
	}
}