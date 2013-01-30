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

import static org.junit.Assert.assertEquals;

import org.apache.etch.util.Log;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;


/**
 * Main implementation for AsyncClient.
 */
public class TestAsyncClient
{
	/** @throws Exception */
	@BeforeClass
	public static void init()
		throws Exception
	{
		Log.addSink( null );
		Log.report( "TestAsyncClient" );
		
		MainAsyncListener.main( new String[] {} );
		
		String uri = "tcp://localhost:4003";

		server = AsyncHelper.newServer( uri, null,
			new AsyncHelper.AsyncClientFactory()
			{
				public AsyncClient newAsyncClient( RemoteAsyncServer server )
					throws Exception
				{
					return new ImplAsyncClient( server );
				}
			} );
		
		server._startAndWaitUp( 4000 );
		up = true;
	}
	
	private static boolean up = false;
	
	/** @throws Exception */
	@Test
	public void sync1() throws Exception
	{
		assertEquals( 0, server.sync( 0 ) );
	}
	
	/** @throws Exception */
	@Test( expected = RuntimeException.class ) // remote side times out
	public void sync2() throws Exception
	{
		server.sync( 1 );
	}
	
	/** @throws Exception */
	@Test
	public void async_queued1() throws Exception
	{
		for (int i = 0; i < 5; i++)
			assertEquals( i, server.async_queued( i ) );
	}
	
	/** @throws Exception */
	@Test( expected = RuntimeException.class ) // remote side times out
	public void async_queued2() throws Exception
	{
		server.async_queued( 5 );
	}
	
	/** @throws Exception */
	@Test
	public void async_free() throws Exception
	{
		for (int i = 0; i < 100; i++)
			assertEquals( i, server.async_free( i ) );
	}
	
	/** @throws Exception */
	@AfterClass
	public static void fini() throws Exception
	{
		if (server != null && up)
			server._stopAndWaitDown( 4000 );
	}
	
	private static RemoteAsyncServer server;
}
