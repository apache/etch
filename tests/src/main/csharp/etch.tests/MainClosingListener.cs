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
using etch.tests;
using Org.Apache.Etch.Bindings.Csharp.Util;
using org.apache.etch.tests;

namespace etch.bindings.csharp.examples.closing
{

///
///<summary> Main implementation for ClosingListener. </summary>
///
public class MainClosingListener : ClosingHelper.ClosingServerFactory
{
	///
 	/// <summary>Main for ClosingListener.</summary>
	/// <param name="args"></param>
	///
	public static void Main(String[] args)
	{
		string uri = "tcp://localhost:4001";
	/*	if (args.Length > 0)
			uri = args[0]; */

		MainClosingListener implFactory = new MainClosingListener();
        ServerFactory listener = ClosingHelper.NewListener(uri, null, implFactory);

		listener.TransportControl( TransportConsts.START_AND_WAIT_UP, 4000 );

        Console.WriteLine("Listener Started");
	}

	///
	/// <summary>Constructs the MainClosingListener.</summary>
	///
	public MainClosingListener()
	{
		// Nothing to do.
	}

	///
	/// <summary>Return a new instance of ClosingServer.</summary>
	/// @param client
	///
	public ClosingServer NewClosingServer( RemoteClosingClient client )
	{
		return new ImplClosingServer( client );
	}
}

}