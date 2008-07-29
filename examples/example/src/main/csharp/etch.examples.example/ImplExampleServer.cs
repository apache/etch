/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

using System;



using etch.examples.example.types.Example;

///<summary>Your custom implementation of BaseExampleServer. Add methods here to provide
///implementation of messages from the client. </summary>
namespace etch.examples.example
{
	///<summary>Implementation for ImplExampleServer</summary>
	public class ImplExampleServer : BaseExampleServer
	{
		/// <summary>Constructs the ImplExampleServer.</summary>
 		/// <param name="client">a connection to the client session. Use this to
 		/// send a message to the client.</param>
		public ImplExampleServer(RemoteExampleClient client)
		{
			this.client = client;
		}
		
		/// <summary>A connection to the client session. Use this to
 		/// send a message to the client.</summary>
		private readonly RemoteExampleClient client;
	
		// TODO: Implement delegates or provide implementation of ExampleServer
		// messages from the client	
	}
}