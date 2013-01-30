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
 * Call to message translator for AsyncClient.
 */
namespace etch.tests
{

	public class ImplAsyncClient : BaseAsyncClient
	{

		/// 
		/// <summary>Constructs the ImplAsyncClient.</summary>
		///
 		/// <param name=server></param>
 		///
		public ImplAsyncClient(RemoteAsyncServer server )
		{
		    this.server = server;
            _delegate_sync = my_sync;
            _delegate_async_free = my_async_free;
            _delegate_async_queued = my_async_queued;

		}

	    private readonly RemoteAsyncServer server;

        ///
        /// <summary>Begins the call to sync.</summary>
        ///
        public int? my_sync(
                int? count
            )
        {
            return count > 0 ? server.sync(count - 1) + 1 : 0;
        }
        ///
        /// <summary>Begins the call to async_queued.</summary>
        ///
        public int? my_async_queued(
                int? count
            )
        {
            return count > 0 ? server.async_queued(count - 1) + 1 : 0;
        }
        ///
        /// <summary>Begins the call to async_free.</summary>
        ///
        public int? my_async_free(
                int? count
            )
        {
            return count > 0 ? server.async_free(count - 1) + 1 : 0;
        }

		
	}
}