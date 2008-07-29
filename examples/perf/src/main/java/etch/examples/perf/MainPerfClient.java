/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.examples.perf;

import java.util.Date;

import etch.bindings.java.support.Mailbox;
import etch.bindings.java.support.ServerFactory;
import etch.examples.perf.Perf.Point;
import etch.examples.perf.PerfHelper.PerfClientFactory;
import etch.examples.perf.PerfHelper.PerfServerFactory;
import etch.util.Assertion;
import etch.util.Log;
import etch.util.PerfTest;
import etch.util.core.io.Transport;

/**
 * Main program for PerfClient. This program makes a connection to the
 * listener created by MainPerfListener.
 */
public class MainPerfClient implements PerfClientFactory, PerfServerFactory
{
	/**
	 * Main program for PerfClient.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "MainPerfClient" );
		
		final String uri = "tcp://localhost:4004";
		final boolean startListener = false;
		
		Transport<ServerFactory> listener;
		
		if (startListener)
		{
			listener= PerfHelper.newListener( uri, null, new MainPerfClient() );
			listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		}
		else
		{
			listener = null;
		}
		
		final MainPerfClient implFactory = new MainPerfClient();
		final int maxWaitTime = 4000;
		
		{
		RemotePerfServer server = PerfHelper.newServer( uri, null, implFactory );
		
		// make sure server is working
		
		server._startAndWaitUp( maxWaitTime );
		
		Assertion.check( server.add( 2, 3 ) == 5, "server.add( 2, 3 ) == 5" );

		Assertion.check( server.add( 3, 4 ) == 7, "server.add( 3, 4 ) == 7" );
		
		Assertion.check( server.sum( new int[] { 1, 2, 3 } ) == 6,
			"server.sum( new int[] { 1, 2, 3 } ) == 6" );
	
		Assertion.check( server.sum( new int[] { 2, 3, 4 } ) == 9,
			"server.sum( new int[] { 2, 3, 4 } ) == 9" );
		
		server.report( 18, "starting" );
		
		server.report( 19, "stopping" );
		
		Point r = server.dist( new Point( 1, 2 ), new Point( 3, 5 ) );
		Assertion.check( r.x == 2, "r.x == 2" );
		Assertion.check( r.y == 3, "r.y == 3" );
		
		r = server.dist( new Point( 10, 9 ), new Point( 4, 5 ) );
		Assertion.check( r.x == -6, "r.x == -6" );
		Assertion.check( r.y == -4, "r.y == -4" );
		
		Date t0 = new Date();
		long adj = 3600000L;
		
		Date t1 = server.add2( t0, adj );
		Assertion.check( (t1.getTime()-t0.getTime()) == adj,
			String.format( "server.add2( %s, %s ) => %s [bogus!]", t0, adj, t1 ) );
		
		t1 = server.add2( t0, -adj );
		Assertion.check( (t1.getTime()-t0.getTime()) == -adj,
			String.format( "server.add2( %s, %s ) => %s [bogus!]", t0, -adj, t1 ) );
		
		server.report2( new Date(), 20, "begin" );
		
		server.report2( new Date(), 21, "end" );

		server._stopAndWaitDown( maxWaitTime );
		}
		
		// run tests
		
		final int runtime = 60;
		final int trials = 3;
		final boolean full = false;
		
		for (int nThreads: new int[] { 1, 1 })
		{
			new PerfTest( "add", runtime, trials, nThreads )
			{
				@Override
				public void run( long n ) throws Exception
				{
					RemotePerfServer server = PerfHelper.newServer( uri, null, implFactory );
					server._startAndWaitUp( maxWaitTime );
					
					while (n-- > 0)
						server.add( 1000000000, 2000000000 );
					
					server._stopAndWaitDown( maxWaitTime );
				}
			}.run();

			new PerfTest( "async add", runtime, trials, nThreads )
			{
				@Override
				public void run( long n ) throws Exception
				{
					RemotePerfServer server = PerfHelper.newServer( uri, null, implFactory );
					server._startAndWaitUp( maxWaitTime );
					
					if (n-- > 0)
					{
						Mailbox mb1 = server._async._begin_add( 1000000000, 2000000000 );
						while (n-- > 0)
						{
							Mailbox mb2 = server._async._begin_add( 1000000000, 2000000000 );
							server._async._end_add( mb1 );
							mb1 = mb2;
						}
						server._async._end_add( mb1 );
					}
					
					server._stopAndWaitDown( maxWaitTime );
				}
			}.run();
		
			new PerfTest( "report", runtime, trials, nThreads )
			{
				@Override
				public void run( long n ) throws Exception
				{
					RemotePerfServer server = PerfHelper.newServer( uri, null, implFactory );
					server._startAndWaitUp( maxWaitTime );
					
					while (n-- > 0)
						server.report( 23,
							// 50 characters:
							"this message describes the specifics of a code 23." );
					
					// sync up with message stream.
					Assertion.check( server.add( 8, 9 ) == 17, "server.add( 8, 9 ) == 17" );
	
					server._stopAndWaitDown( maxWaitTime );
				}
			}.run();
		
			if (full)
			{
				final int[] values = new int[1000];
				for (int i = 0; i < values.length; i++)
					values[i] = i;
				
				new PerfTest( "sum-"+values.length, runtime, trials, nThreads )
				{
					@Override
					public void run( long n ) throws Exception
					{
						RemotePerfServer server = PerfHelper.newServer( uri, null, implFactory );
						server._startAndWaitUp( maxWaitTime );
						
						while (n-- > 0)
							server.sum( values );
		
						server._stopAndWaitDown( maxWaitTime );
					}
				}.run();
				
				new PerfTest( "dist", runtime, trials, nThreads )
				{
					@Override
					public void run( long n ) throws Exception
					{
						RemotePerfServer server = PerfHelper.newServer( uri, null, implFactory );
						server._startAndWaitUp( maxWaitTime );
						
						while (n-- > 0)
							server.dist( new Point( 1, 2 ), new Point( 1000000000, 2000000000 ) );
		
						server._stopAndWaitDown( maxWaitTime );
					}
				}.run();
			
				new PerfTest( "add2", runtime, trials, nThreads )
				{
					@Override
					public void run( long n ) throws Exception
					{
						RemotePerfServer server = PerfHelper.newServer( uri, null, implFactory );
						server._startAndWaitUp( maxWaitTime );
						
						long adj = 365*24*60*60*1000L; // 1 year
						
						while (n-- > 0)
							server.add2( new Date(), adj );
		
						server._stopAndWaitDown( maxWaitTime );
					}
				}.run();
			
				new PerfTest( "report2", runtime, trials, nThreads )
				{
					@Override
					public void run( long n ) throws Exception
					{
						RemotePerfServer server = PerfHelper.newServer( uri, null, implFactory );
						server._startAndWaitUp( maxWaitTime );
						
						while (n-- > 0)
							server.report2( new Date(), 23,
								// 50 characters:
								"this message describes the specifics of a code 23." );
						
						// sync up with message stream.
						Assertion.check( server.add( 8, 9 ) == 17, "server.add( 8, 9 ) == 17" );
		
						server._stopAndWaitDown( maxWaitTime );
					}
				}.run();
			}
		}
		
		if (listener != null)
			listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
	}

	public PerfClient newPerfClient( RemotePerfServer server )
		throws Exception
	{
		return new ImplPerfClient( server );
	}

	public PerfServer newPerfServer( RemotePerfClient client ) throws Exception
	{
		return new ImplPerfServer( client );
	}
}
