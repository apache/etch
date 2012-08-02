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

namespace org.apache.etch.examples.helloworld
{
	///<summary>Main implementation for HelloWorldClient.</summary>
	public class MainHelloWorldClient : HelloWorldHelper.HelloWorldClientFactory
	{
		///<summary>Main for HelloWorldClient</summary>
		///<param name="args">Command Line Arguments</param>
		public static void Main(String[] args)
		{
			// TODO: Change to correct URI
			//string uri = "udp://127.0.0.1:4001";
			string uri = "tcp://127.0.0.1:4001";
			
			RemoteHelloWorldServer server = HelloWorldHelper.NewServer( uri, null, new MainHelloWorldClient());
		
			// Connect to the service
			server._StartAndWaitUp( 4000 );

			org.apache.etch.examples.helloworld.types.HelloWorld.user theUser = new user(5,"User");
	            	string s = server.say_hello(theUser);
			Console.Write(s);

			// Disconnect from the service
			server._StopAndWaitDown( 4000 );			
		}

		///<summary>Return a new instance of HelloWorldClient.</summary>
		///<param name="server">Reference to remote service</param>
		///<returns>Client Implementation</returns>
		public HelloWorldClient NewHelloWorldClient( RemoteHelloWorldServer server )
		{
			return new ImplHelloWorldClient( server );
		}
	}
}
