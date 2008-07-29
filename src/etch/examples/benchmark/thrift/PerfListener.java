package etch.examples.benchmark.thrift;

import java.util.ArrayList;

import com.facebook.thrift.TException;
import com.facebook.thrift.server.TServer;
import com.facebook.thrift.server.TSimpleServer;
import com.facebook.thrift.server.TThreadPoolServer;
import com.facebook.thrift.transport.TServerSocket;

/**
 * Performance test of thrift generated interfaces.
 */
public class PerfListener
{
	/**
	 * Handler for client requests.
	 */
	public static class Handler implements Perf.Iface
	{
		/**
		 * Constructs the Handler.
		 */
		public Handler()
		{
			// nothing to do.
		}

		public int add( int x, int y ) throws TException
		{
			return x + y;
		}

		public void report( int code, String msg ) throws TException
		{
			// ignore.
		}

		public int sum( ArrayList<Integer> values ) throws TException
		{
			int sum = 0;
			for (int value : values)
				sum += value;
			return sum;
		}
	}

	/**
	 * @param args
	 */
	public static void main( String[] args )
	{
		try
		{
			int port = 4002;

			// Processor
			Handler handler = new Handler();
			Perf.Processor processor = new Perf.Processor( handler );

			// Transport
			TServerSocket tServerSocket = new TServerSocket( port );

			TServer serverEngine;

			if (true)
			{
				// Simple Server
				serverEngine = new TSimpleServer( processor, tServerSocket);
			}
			else
			{
				// ThreadPool Server
				serverEngine = new TThreadPoolServer( processor, tServerSocket );
			}
			
			// Run it
			System.out.println( "Starting the server on port " + port + "..." );
			serverEngine.serve();

		}
		catch ( Exception x )
		{
			x.printStackTrace();
		}
		System.out.println( "done." );
	}
}
