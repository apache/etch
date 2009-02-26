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

import static org.junit.Assert.assertEquals;

import org.apache.etch.bindings.java.support._Etch_RuntimeException;
import org.apache.etch.interoptester.example.iot.IOTClient;
import org.apache.etch.interoptester.example.iot.IOTHelper;
import org.apache.etch.interoptester.example.iot.RemoteIOTServer;
import org.junit.BeforeClass;
import org.junit.Test;
import org.junit.runner.Description;
import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.notification.Failure;
import org.junit.runner.notification.RunListener;


/**
 * Main program for IOTClient. This program makes a connection to the
 * listener created by MainIOTListener.
 */
public class MainIOTClient implements IOTHelper.IOTClientFactory
{
	/**
	 * Main program for IOTClient.
	 * 
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		System.out.println( "MainIOTClient: starting" );
		
		if (args.length > 1)
		{
			System.err.println( "usage: MainIOTClient [uri]" );
			System.exit( 1 );
		}
		
		if (args.length > 0)
			uri = args[0];
		
		dontStartListener = true;
		
		JUnitCore c = new org.junit.runner.JUnitCore();
		
		c.addListener( new RunListener()
		{
			@Override
			public void testRunStarted( Description description )
				throws Exception
			{
				System.out.println( "testRunStarted "+description );
			}
			
			@Override
			public void testStarted( Description description ) throws Exception
			{
				System.out.print( "test "+description );
			}
			
			@Override
			public void testFailure( Failure failure ) throws Exception
			{
				System.out.println( "[failed:  "+failure+"]" );
			}
			
			@Override
			public void testIgnored( Description description ) throws Exception
			{
				System.out.println( " [ignored]" );
			}
			
			@Override
			public void testFinished( Description description )
				throws Exception
			{
				System.out.println( " [ok]" );
			}
			
			@Override
			public void testRunFinished( Result result ) throws Exception
			{
				System.out.println( "testRunFinished" );
			}
		} );
		
		Result r = c.run( MainIOTClient.class );
		
		if (!r.wasSuccessful())
		{
			System.err.printf( "MainIOTClient: tests failed (%d/%d/%d)",
				r.getRunCount(), r.getIgnoreCount(), r.getFailureCount() );
			System.err.println();
			System.exit( 1 );
		}
		
		System.out.printf( "MainIOTClient: tests succeeeded (%d/%d)",
			r.getRunCount(), r.getIgnoreCount() );
		System.out.println();
		System.exit( 0 );
	}
	
	private static boolean dontStartListener;
	
	private static String uri = "tcp://localhost:4001";
	
	/** @throws Exception */
	@BeforeClass
	public static void beforeClass() throws Exception
	{
		if (!dontStartListener)
			MainIOTListener.main( new String[] {} );
	}
	
	/** @throws Exception */
	@org.junit.Before
	public void before() throws Exception
	{
		server = IOTHelper.newServer( uri, null, this );
		server._startAndWaitUp( 4000 );
	}
	
	private RemoteIOTServer server;
	
	/** @throws Exception */
	@org.junit.After
	public void after() throws Exception
	{
		server._stopAndWaitDown( 4000 );
		server = null;
	}

	public IOTClient newIOTClient( RemoteIOTServer server )
		throws Exception
	{
		return new ImplIOTClient( server );
	}
	
	/** @throws Exception */
	@Test( expected = _Etch_RuntimeException.class )
	public void add_null_null() throws Exception
	{
		server.add( null, null );
	}
	
	/** @throws Exception */
	@Test( expected = _Etch_RuntimeException.class )
	public void add_null_2() throws Exception
	{
		server.add( null, 2 );
	}
	
	/** @throws Exception */
	@Test( expected = _Etch_RuntimeException.class )
	public void add_1_null() throws Exception
	{
		server.add( 1, null );
	}
	
	/** @throws Exception */
	@Test
	public void add_1_2() throws Exception
	{
		assertEquals( 3, server.add( 1, 2 ) );
	}
}
