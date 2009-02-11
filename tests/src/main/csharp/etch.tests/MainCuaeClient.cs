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
using Org.Apache.Etch.Bindings.Csharp.Util;
using org.apache.etch.tests;
using org.apache.etch.tests.types.Cuae;

namespace etch.tests
{

///
///<summary> Main implementation for CuaeClient. </summary>
///
public class MainCuaeClient : CuaeHelper.CuaeClientFactory
{
	///
	/// <summary>Main for CuaeClient</summary>
	/// <param name="args"></param>
	///
	public static void Main(String[] args)
	{
		string uri = "tcp://localhost:4001";
		if (args.Length > 0)
			uri = args[0];

        MainCuaeListener.Main1(new String[] { });


		MainCuaeClient implFactory = new MainCuaeClient();
		RemoteCuaeServer server = CuaeHelper.NewServer( uri, null, implFactory );
		
		server._TransportControl(TransportConsts.START_AND_WAIT_UP, 4000 );

        Console.WriteLine("--------doit(23)-------------------------------------");
        Console.WriteLine("doit( 23 ) => {0}\n",
            server.doit1(new Request(23)));

        Console.WriteLine("--------doit(23,\"skiddoo\")-------------------------------------");
        Console.WriteLine("doit( 23,\"skiddoo\" ) => {0}\n",
            server.doit1(new ReqWithMessage(23, "skiddoo")));



        Console.WriteLine("------------doit( 19 )-----------------------------");
        Console.WriteLine("doit( 19 ) => {0}\n",
            server.doit1(new Request(19)));

        Console.WriteLine("------------doit( 13 )-----------------------------");
        Console.WriteLine("doit( 13,\"unlucky friday\" ) => {0}\n",
            server.doit1(new ReqWithMessage(13, "unlucky friday")));

        Console.WriteLine("----------------------------------------------");
        Console.WriteLine("doit( 13 ) => {0}\n",
            server.doit1(new Request(13)));

        Console.WriteLine("--- doit( 23 ) -------------------------------------------");
        Console.WriteLine("doit( [23] ) => {0}\n",
            server.doit3(new Request[] { new Request(23) })[0]);


		server._TransportControl(TransportConsts.STOP_AND_WAIT_DOWN, 4000 );

	}

	///
	/// <summary>Constructs the MainCuaeClient.</summary>
	///
	public MainCuaeClient()
	{
		// Nothing to do.
	}

	///
	/// <summary>Return a new instance of CuaeClient.</summary>
	/// @param server
	///
	public CuaeClient NewCuaeClient( RemoteCuaeServer server )
	{
		return new ImplCuaeClient( server );
	}

}

}