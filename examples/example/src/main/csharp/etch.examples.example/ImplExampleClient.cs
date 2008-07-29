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

///<summary>Your custom implementation of BaseExampleClient. Add methods here to provide
///implementation of messages from the server. </summary>
namespace etch.examples.example
{
	///<summary>Implementation for ImplExampleClient</summary>
	public class ImplExampleClient : BaseExampleClient
	{
		/// <summary>Constructs the ImplExampleClient.</summary>
 		/// <param name="server">a connection to the server session. Use this to
 		/// send a message to the server.</param>
		public ImplExampleClient(RemoteExampleServer server)
		{
			this.server = server;
		}
		
		/// <summary>A connection to the server session. Use this to
 		/// send a message to the server.</summary>
		private readonly RemoteExampleServer server;
	
		// TODO: Implement delegates or provide implementation of ExampleClient
		// messages from the server	
	}
}