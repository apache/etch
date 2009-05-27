/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

using System;

using org.apache.etch.interoptester.example.iot.types.IOT;

///<summary>Your custom implementation of BaseIOTServer. Add methods here to provide
///implementation of messages from the client. </summary>
namespace org.apache.etch.interoptester.example.iot
{
	///<summary>Implementation for ImplIOTServer</summary>
	public class ImplIOTServer : BaseIOTServer
	{
		/// <summary>Constructs the ImplIOTServer.</summary>
 		/// <param name="client">a connection to the client session. Use this to
 		/// send a message to the client.</param>
		public ImplIOTServer(RemoteIOTClient client)
		{
			this.client = client;
		}
		
		/// <summary>A connection to the client session. Use this to
 		/// send a message to the client.</summary>
		private readonly RemoteIOTClient client;

        public override int? add(int? x, int? y)
        {
            if (x == null)
                throw new ArgumentNullException("x");
            if (y == null)
                throw new ArgumentNullException("y");
            return x + y;
        }
	}
}