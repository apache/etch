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
using System.Threading;
using etch.tests;
using org.apache.etch.tests;

namespace etch.bindings.csharp.examples.closing
{

///
///<summary> Main implementation for ClosingClient. </summary>
///
public class MainClosingClient : ClosingHelper.ClosingClientFactory
{
	///
	/// <summary>Main for ClosingClient</summary>
	/// <param name="args"></param>
	///
	public static void Main(String[] args)
	{
		string uri = "tcp://localhost:4001";
		if (args.Length > 0)
			uri = args[0];
       


		MainClosingClient implFactory = new MainClosingClient();
		RemoteClosingServer server = ClosingHelper.NewServer( uri, null, implFactory );

        Console.WriteLine("MainClosingClient.startingServer");
        
      
        server._StartAndWaitUp(4000);

        Console.WriteLine("MainClosingClient.serverUp");

        Thread.Sleep(100);

        Console.WriteLine("MainClosingClient.sayingCloseToServer");
        server.close();
        Console.WriteLine("MainClosingClient.saidCloseToServer");

        Thread.Sleep(100);

        Console.WriteLine("MainClosingClient.waitingServerDown");

        server._WaitDown(4000);

        Console.WriteLine("MainClosingClient.serverDown");

	}

	///
	/// <summary>Constructs the MainClosingClient.</summary>
	///
	public MainClosingClient()
	{
		// Nothing to do.
	}

	///
	/// <summary>Return a new instance of ClosingClient.</summary>
	/// @param server
	///
	public ClosingClient NewClosingClient( RemoteClosingServer server )
	{
		return new ImplClosingClient( server );
	}

}

}