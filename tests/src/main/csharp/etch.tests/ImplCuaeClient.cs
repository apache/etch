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
using etch.tests;
using etch.tests.types.Cuae;

/**
 * Call to message translator for CuaeClient.
 */
namespace etch.tests
{

	public class ImplCuaeClient : BaseCuaeClient
	{

		/// 
		/// <summary>Constructs the ImplCuaeClient.</summary>
		///
 		/// <param name=server></param>
 		///
		public ImplCuaeClient(RemoteCuaeServer server )
		{
		    this.server = server;
            _delegate_doit2 = Mydoit2;
		}

	    private readonly RemoteCuaeServer server;

        public Response Mydoit2(
        Request req
    )
        {
            int? reqNo = req.GetCode();
            switch (reqNo.GetValueOrDefault())
            {
                case 7: return new Response("apple");
                case 11: return new RespWithCode("banana", 37);
                case 99: return new Response("prune");
                default: return new RespWithCode("me putz " + req.code + " in teh bukkit", 101);
            }
        }
		
	}
}