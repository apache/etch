package etch.examples.benchmark.etch;

import metreos.core.io.TcpConnection;
import metreos.util.Assertion;
import metreos.util.Log;
import metreos.util.PerfTest;
import etch.bindings.java.support.Pool;

/**
 * Runs performance tests using PerfServer.
 */
public class RunPerf
{
	/**
	 * @param args
	 * @throws Exception 
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "RunPerf" );
		
		Pool queuedPool = null;
		Pool freePool = null;
		
		// connect to server
		
		TcpConnection c = new TcpConnection( null, "", 4001, 0 );
		final PerfServer server = PerfHelper.newPerfServer( c,
			new PerfClient() { /* nothing */ }, queuedPool, freePool );
		
		c.start();
		c.waitUp( 4000 );
		
		// make sure server is working
		
		Assertion.check( server.add( 2, 3 ) == 5, "server.add( 2, 3 ) == 5" );

		Assertion.check( server.add( 3, 4 ) == 7, "server.add( 3, 4 ) == 7" );
		
		Assertion.check( server.sum( new int[] { 1, 2, 3 } ) == 6,
			"server.sum( new int[] { 1, 2, 3 } ) == 6" );
	
		Assertion.check( server.sum( new int[] { 2, 3, 4 } ) == 9,
			"server.sum( new int[] { 2, 3, 4 } ) == 9" );
		
		server.report( 23, "all ok" );
		
		server.report( 19, "stopping" );
		
		int runtime = 60;
		int trials = 3;
		
		new PerfTest( "add", runtime, trials )
		{
			@Override
			public void run( long n )
			{
				while (n-- > 0)
					server.add( 1000000000, 2000000000 );
			}
		}.run();
	
		final int[] values = new int[2];
		for (int i = 0; i < values.length; i++)
			values[i] = i;
		
		new PerfTest( "sum", runtime, trials )
		{
			@Override
			public void run( long n )
			{
				while (n-- > 0)
					server.sum( values );
			}
		}.run();
	
		new PerfTest( "report", runtime, trials )
		{
			@Override
			public void run( long n )
			{
				while (n-- > 0)
					server.report( 23,
						// 50 characters:
						"this message describes the specifics of a code 23." );
			}
		}.run();
	
		((RemotePerfServer) server)._stop();
		c.waitDown( 4000 );
	}
}
