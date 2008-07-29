package etch.examples.benchmark.etch;

import metreos.core.io.Connection;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.util.Log;
import etch.bindings.java.support.Pool;

/**
 * Starts a Perf listener.
 */
public class PerfListener
{
	/**
	 * Program entry point.
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		Log.addSink( null );
		Log.report( "PerfListener" );
		
		Pool queuedPool = null;
		Pool freePool = null;
		
		// create the listener
		
		ListenerHandler lh = PerfHelper.newPerfListener( queuedPool, freePool );
		Connection<ListenerHandler> l = new Listener( lh, 5, null, 4001, 0 );
		l.start();
	}
}
