/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.tests;


/**
 * Your custom implementation of BaseCuaeClient. Add methods here to provide
 * implementations of messages from the server.
 */
public class ImplCuaeClient extends BaseCuaeClient
{
	/**
	 * Constructs the ImplCuaeClient.
	 *
	 * @param server a connection to the server session. Use this to send a
	 * message to the server.
	 */
	public ImplCuaeClient( RemoteCuaeServer server )
	{
		this.server = server;
	}
	
	@SuppressWarnings("unused")
	private final RemoteCuaeServer server;

	@Override
	public Response doit2( Request req )
	{
		switch (req.code)
		{
			case 7: return new Response( "apple" );
			case 11: return new RespWithCode( "banana", 37 );
			case 99: return new Response( "prune" );
			default: return new RespWithCode( "me putz "+req.code+" in teh bukkit", 101 );
		}
	}
}