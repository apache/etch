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

import java.util.Date;

/**
 * Your custom implementation of BaseExampleFooServer. Add methods here to provide
 * implementations of messages from the client.
 */
public class ImplExampleFooServer extends BaseExampleFooServer
{
	/**
	 * Constructs the ImplExampleFooServer.
	 *
	 * @param client a connection to the client session. Use this to send a
	 * message to the client.
	 */
	public ImplExampleFooServer( RemoteExampleFooClient client )
	{
		this.client = client;
	}
	
	/**
	 * A connection to the client session. Use this to send a
	 * message to the client.
	 */
	@SuppressWarnings( "unused" )
	private final RemoteExampleFooClient client;

	@Override
	public Integer helloServer( String msg, Date d )
	{
		return msg.hashCode();
	}
	
	@Override
	public Integer foo( String msg )
	{
		return msg.hashCode();
	}
}