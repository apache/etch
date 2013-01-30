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

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.bindings.java.support.TransportFactory;
import org.apache.etch.bindings.java.transport.TcpTransportFactory;
import org.apache.etch.util.core.io.Transport;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;


/** Test of Test1Helper */
public class TestTest1Helper
{
	private static ServerFactory listener;
	
	/** @throws Exception */
	@BeforeClass
	public static void init() throws Exception
	{
		TransportFactory.define( "rohit", new TcpTransportFactory() );
		
		String uri = "rohit://0.0.0.0:4040";
		
		listener = Test1Helper.newListener( uri, null, new MainTest1Listener() );
		
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}
	
	/** @throws Exception */
	@Test
	public void test() throws Exception
	{
		String uri = "rohit://localhost:4040";

		RemoteTest1Server server = Test1Helper.newServer( uri, null, new MainTest1Client() );
		server._startAndWaitUp( 4000 );

		// TODO nothing to do but wait for server to really be up.
		Thread.sleep( 1000 );
	
		server._stopAndWaitDown( 4000 );
		server = null;
	}
	
	/** @throws Exception */
	@AfterClass
	public static void fini() throws Exception
	{
		listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 0 );
		listener = null;
	}
}
