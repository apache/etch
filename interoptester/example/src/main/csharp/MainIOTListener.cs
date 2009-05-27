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

using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;

using org.apache.etch.interoptester.example.iot.types.IOT;

namespace org.apache.etch.interoptester.example.iot
{
	///<summary>Main implementation for IOTListener.</summary>
	public class MainIOTListener : IOTHelper.IOTServerFactory
	{
 		///<summary>Main for IOTListener.</summary>
		///<param name="args">Command Line Arguments</param>
		public static void Main(String[] args)
        {
            if (args.Length > 1)
            {
                Console.WriteLine("usage: MainIOTListener [uri]");
                return;
            }
		
            string uri = args.Length > 0 ? args[0] : "tcp://localhost:4001";
			
			ServerFactory listener = IOTHelper.NewListener( uri, null, new MainIOTListener());
			
			// Start the Listener
			listener.TransportControl( TransportConsts.START_AND_WAIT_UP, 4000 );
            Console.WriteLine( "listener started: "+listener );
		}

		///<summary>Return a new instance of IOTServer.</summary>
		///<param name="client">Reference to remote service</param>
		///<returns>Server Implementation</returns>
		public IOTServer NewIOTServer( RemoteIOTClient client )
		{
			return new ImplIOTServer( client );
		}
	}
}