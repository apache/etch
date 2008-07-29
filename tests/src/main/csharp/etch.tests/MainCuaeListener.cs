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
using Etch.Util;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;

namespace etch.tests
{

///
///<summary> Main implementation for CuaeListener. </summary>
///
public class MainCuaeListener : MyCuaeHelper.MyCuaeServerFactory
{
	///
 	/// <summary>Main for CuaeListener.</summary>
	/// <param name="args"></param>
	///
	public static void Main1(String[] args)
	{
		string uri = "tcp://localhost:4001";
		/*if (args.Length > 0)
			uri = args[0]; */

		MainCuaeListener implFactory = new MainCuaeListener();
        Transport<ServerFactory> listener = MyCuaeHelper.NewListener(uri, null, implFactory);

		listener.TransportControl( TransportConsts.START_AND_WAIT_UP, 4000 );
	}

	///
	/// <summary>Constructs the MainCuaeListener.</summary>
	///
	public MainCuaeListener()
	{
		// Nothing to do.
	}


    public SessionMessage NewMyCuaeServer(DeliveryService d, Pool queued, Pool free, MyValueFactoryCuae vf)
    {
        return new MyCuaeServerStub(d, queued, free, vf);
    }
}

}