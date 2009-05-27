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
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace org.apache.etch.interoptester.example.iot
{
	///<summary>Main implementation for IOTClient.</summary>
	public class MainIOTClient : IOTHelper.IOTClientFactory
	{
		///<summary>Main for IOTClient</summary>
		///<param name="args">Command Line Arguments</param>
		public static void Main(String[] args)
        {
            if (args.Length > 1)
            {
                Console.WriteLine("usage: MainIOTClient [uri]");
                return;
            }
		
			string uri = args.Length > 0 ? args[0] : "tcp://localhost:4001";
			
			RemoteIOTServer server = IOTHelper.NewServer( uri, null, new MainIOTClient());
		
			// Connect to the service
			server._StartAndWaitUp( 4000 );

            Assertion.Check(server.add(2, 3) == 5, "server.add(2, 3) == 5");

            try
            {
                server.add(null, 3);
                Assertion.Check(false, "false");
            }
            catch
            {
                // ignore.
            }

            try
            {
                server.add(2, null);
                Assertion.Check(false, "false");
            }
            catch
            {
                // ignore.
            }

            try
            {
                server.add(null, null);
                Assertion.Check(false, "false");
            }
            catch
            {
                // ignore.
            }

			// Disconnect from the service
			server._StopAndWaitDown( 4000 );
            Console.WriteLine("done");
		}

		///<summary>Return a new instance of IOTClient.</summary>
		///<param name="server">Reference to remote service</param>
		///<returns>Client Implementation</returns>
		public IOTClient NewIOTClient( RemoteIOTServer server )
		{
			return new ImplIOTClient( server );
		}
	}
}