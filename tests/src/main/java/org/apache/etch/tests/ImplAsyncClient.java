/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.tests;


/**
 * Your custom implementation of BaseAsyncClient. Add methods here to provide
 * implementations of messages from the server.
 */
public class ImplAsyncClient extends BaseAsyncClient
{
	/**
	 * Constructs the ImplAsyncClient.
	 *
	 * @param server a connection to the server session. Use this to send a
	 * message to the server.
	 */
	public ImplAsyncClient( RemoteAsyncServer server )
	{
		this.server = server;
	}
	
	private final RemoteAsyncServer server;

	public Integer sync( Integer count )
	{
		return count > 0 ? server.sync( count - 1 ) + 1 : 0;
	}

	public Integer async_queued( Integer count )
	{
		return count > 0 ? server.async_queued( count - 1 ) + 1 : 0;
	}

	public Integer async_free( Integer count )
	{
		return count > 0 ? server.async_free( count - 1 ) + 1 : 0;
	}
}