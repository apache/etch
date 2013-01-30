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

import java.util.Date;
import java.util.List;
import java.util.Map;
import java.util.Set;


/**
 * Your custom implementation of BaseTypesServer. Add methods here to provide
 * implementations of messages from the client.
 */
@SuppressWarnings("unchecked")
public class ImplTypesServer extends BaseTypesServer
{
	/**
	 * Constructs the ImplTypesServer.
	 *
	 * @param client a connection to the client session. Use this to send a
	 * message to the client.
	 */
	public ImplTypesServer( RemoteTypesClient client )
	{
		this.client = client;
	}
	
	@SuppressWarnings("unused")
	private final RemoteTypesClient client;

	@Override
	public Map test1( Map map )
	{
		System.out.println( "map = "+map );
		return map;
	}
	
	@Override
	public Set test2( Set set )
	{
		System.out.println( "set = "+set );
		return set;
	}
	
	@Override
	public List test3( List list )
	{
		System.out.println( "list = "+list );
		return list;
	}
	
	@Override
	public Date test4( Date datetime )
	{
		System.out.println( "datetime = "+datetime );
		return datetime;
	}
}