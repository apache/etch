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
using System.Runtime.CompilerServices;
using etch.tests;
using Etch.Util;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;

namespace etch.tests
{

///
///<summary> Main implementation for AsyncListener. </summary>
///
public class MainAsyncListener : AsyncHelper.AsyncServerFactory
{
	///
 	/// <summary>Main for AsyncListener.</summary>
	/// <param name="args"></param>
	///
	public static void Main(String[] args)
	{
		string uri = "tcp://localhost:4001";
		if (args.Length > 0)
			uri = args[0];

		MainAsyncListener implFactory = new MainAsyncListener();
        Transport<ServerFactory> listener = AsyncHelper.NewListener(uri, null, implFactory);

		listener.TransportControl( TransportConsts.START_AND_WAIT_UP, 4000 );
	}

	///
	/// <summary>Constructs the MainAsyncListener.</summary>
	///
	public MainAsyncListener()
	{
		// Nothing to do.
	}

	///
	/// <summary>Return a new instance of AsyncServer.</summary>
	/// @param client
	///
	public AsyncServer NewAsyncServer( RemoteAsyncClient client )
	{
		return new ImplAsyncServer( client );
	}
}

}