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

import org.apache.etch.bindings.java.support.Mailbox;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.examples.perf.Perf.Point;
import org.apache.etch.examples.perf.PerfHelper.PerfClientFactory;
import org.apache.etch.examples.perf.PerfHelper.PerfServerFactory;
import org.apache.etch.util.Assertion;
import org.apache.etch.util.Log;
import org.apache.etch.util.PerfTest;
import org.apache.etch.util.core.io.Transport;


/**
 * Main program for PerfClient. This program makes a connection to the listener
 * created by MainPerfListener.
 */
public class MainPerfClient implements PerfClientFactory, PerfServerFactory
{
	private static String uri;

	private static int maxWaitTime;

	private static int runtime;

	private static int trials;

	private static boolean full;

	private static int[] threadCounts;

	private static boolean startListener;

	private static MainPerfClient implFactory;

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

		uri = "tcp://localhost:4004";
		maxWaitTime = 4000;
		runtime = 4;
		trials = 3;
		full = false;
		threadCounts = new int[]
		{
			1
		};
		startListener = args.length == 1 && args[0].equals( "startListener" );

		implFactory = new MainPerfClient();

		final ServerFactory listener;

		if (startListener)
		{
			listener = PerfHelper.newListener( uri, null, implFactory );
			listener.transportControl( Transport.START_AND_WAIT_UP, maxWaitTime );
		}
		else
		{
			listener = null;
		}

		// make sure test interfaces work.
		
		qualify();

		// run tests

		for (int nThreads : threadCounts)
		{
			perfAdd( nThreads );
			perfAsyncAdd( nThreads );
			perfReport( nThreads );
			if (full)
			{
				perfSum( nThreads );
				perfDist( nThreads );
				perfAdd2( nThreads );
				perfReport2( nThreads );
			}
		}

		if (listener != null)
			listener.transportControl( Transport.STOP_AND_WAIT_DOWN, maxWaitTime );
	}

	private static void perfReport2( int threads ) throws Exception
	{
		new PerfTest( "report2", runtime, trials, threads )
		{
			@Override
			public void run( long n ) throws Exception
			{
				RemotePerfServer server = startServer();
				
				final Date d = new Date();
				
				while (n-- > 0)
					server
						.report2( d, 23,
						// 50 characters:
							"this message describes the specifics of a code 23." );

				// sync up with message stream.
				Assertion.check( server.add( 8, 9 ) == 17,
					"server.add( 8, 9 ) == 17" );

				stopServer( server );
			}
		}.run();
	}

	private static void perfAdd2( int threads ) throws Exception
	{
		new PerfTest( "add2", runtime, trials, threads )
		{
			@Override
			public void run( long n ) throws Exception
			{
				RemotePerfServer server = startServer();

				final Date d = new Date();
				final long adj = 3600000L; // 1 hour

				while (n-- > 0)
					server.add2( d, adj );

				stopServer( server );
			}
		}.run();
	}

	private static void perfDist( int threads ) throws Exception
	{
		new PerfTest( "dist", runtime, trials, threads )
		{
			@Override
			public void run( long n ) throws Exception
			{
				RemotePerfServer server = startServer();
				
				final int X = 1000000000;
				final int Y = 2000000000;

				while (n-- > 0)
					server.dist( new Point( 1, 2 ), new Point( X, Y ) );

				stopServer( server );
			}
		}.run();
	}

	private static void perfSum( int threads ) throws Exception
	{
		final int[] values = new int[1000];
		for (int i = 0; i < values.length; i++)
			values[i] = i;

		new PerfTest( "sum-" + values.length, runtime, trials, threads )
		{
			@Override
			public void run( long n ) throws Exception
			{
				RemotePerfServer server = startServer();

				while (n-- > 0)
					server.sum( values );

				stopServer( server );
			}
		}.run();
	}

	private static void perfReport( int threads ) throws Exception
	{
		new PerfTest( "report", runtime, trials, threads )
		{
			@Override
			public void run( long n ) throws Exception
			{
				RemotePerfServer server = startServer();

				while (n-- > 0)
					server.report( 23,
					// 50 characters:
						"this message describes the specifics of a code 23." );

				// sync up with message stream.
				Assertion.check( server.add( 8, 9 ) == 17,
					"server.add( 8, 9 ) == 17" );

				stopServer( server );
			}
		}.run();
	}

	private static void perfAsyncAdd( int threads ) throws Exception
	{
		new PerfTest( "async add", runtime, trials, threads )
		{
			@Override
			public void run( long n ) throws Exception
			{
				RemotePerfServer server = startServer();
				
				final int X = 1000000000;
				final int Y = 2000000000;

				if (n-- > 0)
				{
					Mailbox mb1 = server._async._begin_add( X, Y );
					while (n-- > 0)
					{
						Mailbox mb2 = server._async._begin_add( X, Y );
						server._async._end_add( mb1 );
						mb1 = mb2;
					}
					server._async._end_add( mb1 );
				}

				stopServer( server );
			}
		}.run();
	}

	private static void perfAdd( int threads ) throws Exception
	{
		new PerfTest( "add", runtime, trials, threads )
		{
			@Override
			public void run( long n ) throws Exception
			{
				RemotePerfServer server = startServer();
				
				final int X = 1000000000;
				final int Y = 2000000000;

				while (n-- > 0)
					server.add( X, Y );

				stopServer( server );
			}
		}.run();
	}

	private static RemotePerfServer startServer() throws Exception
	{
		RemotePerfServer server = PerfHelper.newServer( uri, null, implFactory );
		server._startAndWaitUp( maxWaitTime );
		return server;
	}

	private static void stopServer( RemotePerfServer server ) throws Exception
	{
		server._stopAndWaitDown( maxWaitTime );
	}

	private static void qualify() throws Exception
	{
		RemotePerfServer server = startServer();

		Assertion.check( server.add( 2, 3 ) == 5, "server.add( 2, 3 ) == 5" );

		Assertion.check( server.add( 3, 4 ) == 7, "server.add( 3, 4 ) == 7" );

		Assertion.check( server.sum( new int[]
		{
			1, 2, 3
		} ) == 6, "server.sum( new int[] { 1, 2, 3 } ) == 6" );

		Assertion.check( server.sum( new int[]
		{
			2, 3, 4
		} ) == 9, "server.sum( new int[] { 2, 3, 4 } ) == 9" );

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
		Assertion.check( t1.getTime() - t0.getTime() == adj, String.format(
			"server.add2( %s, %s ) => %s [bogus!]", t0, adj, t1 ) );

		t1 = server.add2( t0, -adj );
		Assertion.check( t1.getTime() - t0.getTime() == -adj, String.format(
			"server.add2( %s, %s ) => %s [bogus!]", t0, -adj, t1 ) );

		server.report2( new Date(), 20, "begin" );

		server.report2( new Date(), 21, "end" );

		stopServer( server );
	}

	public PerfClient newPerfClient( RemotePerfServer server ) throws Exception
	{
		return new ImplPerfClient( server );
	}

	public PerfServer newPerfServer( RemotePerfClient client ) throws Exception
	{
		return new ImplPerfServer( client );
	}
}
