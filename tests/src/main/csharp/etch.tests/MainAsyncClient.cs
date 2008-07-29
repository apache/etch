// $Id$
// 
// Copyright 2007-2008 Cisco Systems Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using etch.tests;
using Etch.Util;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;

namespace etch.tests
{

///
///<summary> Main implementation for AsyncClient. </summary>
///
public class MainAsyncClient : AsyncHelper.AsyncClientFactory
{
	///
	/// <summary>Main for AsyncClient</summary>
	/// <param name="args"></param>
	///
	public static void Main(String[] args)
	{
		string uri = "tcp://localhost:4001";
		if (args.Length > 0)
			uri = args[0];

		MainAsyncClient implFactory = new MainAsyncClient();
		RemoteAsyncServer server = AsyncHelper.NewServer( uri, null, implFactory );
		
		server._TransportControl(TransportConsts.START_AND_WAIT_UP, 4000 );

		// Insert Your Code Here
		//---------------------------------------------------------------------

		server._TransportControl(TransportConsts.STOP_AND_WAIT_DOWN, 4000 );

	}

	///
	/// <summary>Constructs the MainAsyncClient.</summary>
	///
	public MainAsyncClient()
	{
		// Nothing to do.
	}

	///
	/// <summary>Return a new instance of AsyncClient.</summary>
	/// @param server
	///
	public AsyncClient NewAsyncClient( RemoteAsyncServer server )
	{
		return new ImplAsyncClient( server );
	}

}

}