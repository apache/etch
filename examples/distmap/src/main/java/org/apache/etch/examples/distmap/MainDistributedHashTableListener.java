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

package org.apache.etch.examples.distmap;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.util.core.io.Transport;



/**
 * Main program for DistributedHashTableServer. This program makes a listener to accept
 * connections from MainDistributedHashTableClient.
 */
public class MainDistributedHashTableListener implements DistributedHashTableHelper.DistributedHashTableServerFactory
{
	/**
	 * Main program for DistributedHashTableServer.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String uri = "tcp://0.0.0.0:4007";
		
		ServerFactory listener = DistributedHashTableHelper.newListener( uri, null,
			new MainDistributedHashTableListener() );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}
	
	private final Map<String, Object> map =
		Collections.synchronizedMap( new HashMap<String, Object>() );

	public DistributedHashTableServer newDistributedHashTableServer( RemoteDistributedHashTableClient client )
	{
		return new ImplDistributedHashTableServer( client, map );
	}
}
