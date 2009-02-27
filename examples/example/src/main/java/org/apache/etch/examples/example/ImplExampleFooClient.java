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

package org.apache.etch.examples.example;

/**
 * Your custom implementation of BaseExampleFooClient. Add methods here to provide
 * implementations of messages from the server.
 */
public class ImplExampleFooClient extends BaseExampleFooClient
{
	/**
	 * Constructs the ImplExampleFooClient.
	 *
	 * @param server a connection to the server session. Use this to send a
	 * message to the server.
	 */
	public ImplExampleFooClient( RemoteExampleFooServer server )
	{
		this.server = server;
	}
	
	/**
	 * A connection to the server session. Use this to send a
	 * message to the server.
	 */
	@SuppressWarnings( "unused" )
	private final RemoteExampleFooServer server;

	// TODO insert methods here to provide implementations of ExampleFooClient
	// messages from the server.
	
	@Override
	public void _sessionNotify( Object event ) throws Exception
	{
		if (event instanceof Throwable)
			((Throwable) event).printStackTrace();
		else
			System.out.println( "event = "+event );
	}
}