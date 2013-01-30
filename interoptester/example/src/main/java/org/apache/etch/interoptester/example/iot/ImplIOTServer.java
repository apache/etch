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

package org.apache.etch.interoptester.example.iot;

import org.apache.etch.interoptester.example.iot.BaseIOTServer;
import org.apache.etch.interoptester.example.iot.RemoteIOTClient;

/**
 * Your custom implementation of BaseIOTServer. Add methods here to provide
 * implementations of messages from the client.
 */
public class ImplIOTServer extends BaseIOTServer
{
	/**
	 * Constructs the ImplIOTServer.
	 *
	 * @param client a connection to the client session. Use this to send a
	 * message to the client.
	 */
	public ImplIOTServer( RemoteIOTClient client )
	{
		this.client = client;
	}
	
	/**
	 * A connection to the client session. Use this to send a
	 * message to the client.
	 */
	@SuppressWarnings( "unused" )
	private final RemoteIOTClient client;

	@Override
	public Integer add( Integer x, Integer y )
	{
		if (x == null)
			throw new IllegalArgumentException( "x == null" );
		if (y == null)
			throw new IllegalArgumentException( "y == null" );
		return x + y;
	}
	
	@Override
	public void _sessionNotify( Object event ) throws Exception
	{
		// ignore events.
	}
}