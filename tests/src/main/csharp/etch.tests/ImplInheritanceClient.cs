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
using org.apache.etch.tests;

/**
 * Call to message translator for InheritanceClient.
 */
namespace etch.tests
{

	public class ImplInheritanceClient : BaseInheritanceClient
	{

		/// 
		/// <summary>Constructs the ImplInheritanceClient.</summary>
		///
 		/// <param name=server></param>
 		///
		public ImplInheritanceClient(RemoteInheritanceServer server )
		{
		    this.server = server;
		}

	    private readonly RemoteInheritanceServer server;
	
		// Implement delegates or provide implementation of server
		
	}
}