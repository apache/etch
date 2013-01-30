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

import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.Pool;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.util.core.io.Transport;


/**
 * Main program for CuaeServer. This program makes a listener to accept
 * connections from MainCuaeClient.
 */
public class MainCuaeListener implements MyCuaeHelper.MyCuaeServerFactory
{
	/**
	 * Main program for CuaeServer.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String uri = "tcp://0.0.0.0:4006";

		ServerFactory listener = MyCuaeHelper.newListener( uri, null, new MainCuaeListener() );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}

	public SessionMessage newMyCuaeServer( DeliveryService d, Pool queued, Pool free,
		MyValueFactoryCuae vf )
	{
		return new MyCuaeServerStub( d, queued, free, vf );
	}
}
