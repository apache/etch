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

package org.apache.etch.services.config.example;

/**
 * Your custom implementation of BaseConfigExampleClient. Add methods here to provide
 * implementations of messages from the server.
 */
public class ImplConfigExampleClient extends BaseConfigExampleClient
{
	/**
	 * Constructs the ImplConfigExampleClient.
	 *
	 * @param server a connection to the server session. Use this to send a
	 * message to the server.
	 */
	public ImplConfigExampleClient( RemoteConfigExampleServer server )
	{
		this.server = server;
	}
	
	/**
	 * A connection to the server session. Use this to send a
	 * message to the server.
	 */
	@SuppressWarnings( "unused" )
	private final RemoteConfigExampleServer server;

	// TODO insert methods here to provide implementations of ConfigExampleClient
	// messages from the server.
}