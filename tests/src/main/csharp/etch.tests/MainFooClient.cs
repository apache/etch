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
using org.apache.etch.tests.types.Bar;
using org.apache.etch.tests.types.Baz;
using org.apache.etch.tests.types.Foo;

namespace etch.tests
{

///
///<summary> Main implementation for FooClient. </summary>
///
public class MainFooClient : FooHelper.FooClientFactory
{
	///
	/// <summary>Main for FooClient</summary>
	/// <param name="args"></param>
	///
	public static void Main(String[] args)
	{
		string uri = "tcp://localhost:4001";
        MainFooListener.Main1(null);

		MainFooClient implFactory = new MainFooClient();
		RemoteFooServer server = FooHelper.NewServer( uri, null, implFactory );
		
		server._TransportControl(TransportConsts.START_AND_WAIT_UP, 4000 );

        // Insert Your Code Here
        //---------------------------------------------------------------------
        StrStrHashMap bazTable = new StrStrHashMap();
        bazTable.Add("def", "baz");

        StrStrHashMap barTable = new StrStrHashMap();
        barTable.Add("def", "bar");

        StrStrHashMap fooTable = new StrStrHashMap();
        fooTable.Add("def", "foo");

        BazData bazData = new BazData(1, bazTable);
        BarData barData = new BarData(2, barTable, bazData);
        FooData fooData = new FooData(3, fooTable, barData, bazData);

        bool? ok;

        ok = server.baz1(bazData);
        Console.WriteLine("baz1 => " + ok);

        ok = server.baz2(bazData);
        Console.WriteLine("baz2 => " + ok);

        ok = server.bar1(barData);
        Console.WriteLine("bar1 => " + ok);

        ok = server.bar2(barData);
        Console.WriteLine("bar2 => " + ok);

        ok = server.foo1(fooData);
        Console.WriteLine("foo1 => " + ok);

        ok = server.foo2(fooData);
        Console.WriteLine("foo2 => " + ok);

		server._TransportControl(TransportConsts.STOP_AND_WAIT_DOWN, 4000 );

	}

	///
	/// <summary>Constructs the MainFooClient.</summary>
	///
	public MainFooClient()
	{
		// Nothing to do.
	}

	///
	/// <summary>Return a new instance of FooClient.</summary>
	/// @param server
	///
	public FooClient NewFooClient( RemoteFooServer server )
	{
		return new ImplFooClient( server );
	}

}

}