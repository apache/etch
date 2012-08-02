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



using org.apache.etch.examples.helloworld.types.HelloWorld;

///<summary>Your custom implementation of BaseHelloWorldServer. Add methods here to provide
///implementation of messages from the client. </summary>
namespace org.apache.etch.examples.helloworld
{
	///<summary>Implementation for ImplHelloWorldServer</summary>
	public class ImplHelloWorldServer : BaseHelloWorldServer
	{
		/// <summary>Constructs the ImplHelloWorldServer.</summary>
 		/// <param name="client">a connection to the client session. Use this to
 		/// send a message to the client.</param>
		public ImplHelloWorldServer(RemoteHelloWorldClient client)
		{
			this.client = client;
		}
		
		/// <summary>A connection to the client session. Use this to
 		/// send a message to the client.</summary>
		private readonly RemoteHelloWorldClient client;
	
		public override string say_hello (
				org.apache.etch.examples.helloworld.types.HelloWorld.user to_whom
		)
		{
			return "Hello " + to_whom.name;
		}
	}
}
