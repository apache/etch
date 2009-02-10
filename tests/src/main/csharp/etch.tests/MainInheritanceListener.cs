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
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;
using org.apache.etch.tests;

namespace etch.tests
{

///
///<summary> Main implementation for InheritanceListener. </summary>
///
public class MainInheritanceListener : InheritanceHelper.InheritanceServerFactory
{ 
	///
 	/// <summary>Main for InheritanceListener.</summary>
	/// <param name="args"></param>
	///
	public static void Main(String[] args)
	{
		string uri = "tcp://localhost:4001";
		if (args.Length > 0)
			uri = args[0];

		MainInheritanceListener implFactory = new MainInheritanceListener();
        ServerFactory listener = InheritanceHelper.NewListener(uri, null, implFactory);

		listener.TransportControl( TransportConsts.START_AND_WAIT_UP, 4000 );
	}

	///
	/// <summary>Constructs the MainInheritanceListener.</summary>
	///
	public MainInheritanceListener()
	{
		// Nothing to do.
	}

	///
	/// <summary>Return a new instance of InheritanceServer.</summary>
	/// @param client
	///
	public InheritanceServer NewInheritanceServer( RemoteInheritanceClient client )
	{
		return new ImplInheritanceServer( client );
	}
}

}