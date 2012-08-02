/* $Id: TestHelloWorldServer.java 743147 2009-02-10 22:53:01Z sccomer $
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

package org.apache.etch.examples.helloworld;

import java.util.Date;

import junit.framework.Assert;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.examples.helloworld.HelloWorld.user;
import org.apache.etch.util.Log;
import org.apache.etch.util.core.io.Transport;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;


/** */
public class TestHelloWorldServer
{

	/** transport */
	public static ServerFactory listener;
	
	RemoteHelloWorldServer server;

	/**
	 * @throws Exception
	 */
	@BeforeClass
	public static void startListener() throws Exception
	{
		Log.addSink(null);
		Log.report("StartExampleListener");
			
		String uri = "tcp://localhost:4009";
		MainHelloWorldListener implFactory = new MainHelloWorldListener();
		listener = HelloWorldHelper.newListener( uri, null, implFactory );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	
		Log.report( "StartExampleListener",
				"status", "up",
				"listener", listener );
	}

	/**
	 * @throws Exception
	 */
	@AfterClass
	public static void stopListener() throws Exception
	{
		if (listener != null)
		{
			listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
			listener = null;
		}
	}
	
	/**
	 * Connect client to serve
	 * @throws Exception
	 */
	@Before
	public void makeConnection()
		throws Exception
	{
		String uri = "tcp://localhost:4009";

		server = HelloWorldHelper.newServer( uri, null, new MainHelloWorldClient() );

		server._startAndWaitUp( 4000 );
	}

	/**
	 * Shutdown connection to server
	 * @throws Exception
	 */
	@After
	public void shutConnection()
		throws Exception
	{
		if (server != null)
		{
			server._stop();
		}
	}
	
	/**
	 * @throws Exception
	 */
	@Test
	public void checkConnection() throws Exception
	{
		Assert.assertNotNull(server);
	}
	
	/**
	 * @throws Exception
	 */
	@Test
	public void testSayHello() throws Exception
	{
		user u = new user(5, "Test");
		String result = server.say_hello(u);
		Assert.assertTrue(result.equals("Hello Test"));
	}
	


}
