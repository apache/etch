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

using Etch.Util;

using System;

using etch.tests;


/**
 * Call to message translator for FooClient.
 */
namespace etch.tests
{

	public class ImplFooClient : BaseFooClient
	{

		/// 
		/// <summary>Constructs the ImplFooClient.</summary>
		///
 		/// <param name=server></param>
 		///
		public ImplFooClient(RemoteFooServer server )
		{
		    this.server = server;
		}

	    private readonly RemoteFooServer server;
	
		// Implement delegates or provide implementation of server
		
	}
}