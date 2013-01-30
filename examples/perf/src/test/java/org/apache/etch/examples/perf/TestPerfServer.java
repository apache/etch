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

package org.apache.etch.examples.perf;

import java.util.Date;

import junit.framework.Assert;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.examples.perf.Perf.Point;
import org.apache.etch.util.Log;
import org.apache.etch.util.core.io.Transport;
import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;


/** */
public class TestPerfServer
{

	/** transport */
	public static ServerFactory listener;
	
	RemotePerfServer server;

	/**
	 * @throws Exception
	 */
	@BeforeClass
	public static void startListener() throws Exception
	{
		Log.addSink(null);
		Log.report("StartExampleListener");
			
		String uri = "tcp://localhost:4009";
		MainPerfListener implFactory = new MainPerfListener();
		listener = PerfHelper.newListener( uri, null, implFactory );

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

		server = PerfHelper.newServer( uri, null, new MainPerfClient() );

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
	public void testAdd() throws Exception
	{
		int result = server.add( 3, 5 );
		Assert.assertEquals( 8, result);
	}
	
	/**
	 * @throws Exception
	 */
	@Test
	public void testAdd2() throws Exception
	{
		Date d = new Date();
		long ms = 1000;
		Date retDate = server.add2( d, ms );
		Assert.assertEquals( new Date(d.getTime() + ms), retDate );
	}
	
	/**
	 * @throws Exception
	 */
	@Test
	public void testSum() throws Exception
	{
		int[] a = {1,2,3};
		int result = server.sum( a );
		Assert.assertEquals( 6, result );
	}
	
	/**
	 * @throws Exception
	 */
	@Test
	public void testDistance() throws Exception
	{
		Point a = new Point(2,3);
		Point b = new Point(4,5);
		Point result = server.dist( a, b );
		Assert.assertEquals( 2, result.getX().intValue() );
		Assert.assertEquals( 2, result.getY().intValue() );
		
	}
	


}
