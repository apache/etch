package etch.examples.benchmark.thrift;

import java.util.ArrayList;

import metreos.util.Assertion;
import metreos.util.PerfTest;

import com.facebook.thrift.TException;
import com.facebook.thrift.protocol.TBinaryProtocol;
import com.facebook.thrift.transport.TSocket;
import com.facebook.thrift.transport.TTransport;

/**
 * Test Java client for thrift. Essentially just a copy of the C++ version,
 * this makes a variety of requests to enable testing for both performance and
 * correctness of the output.
 *
 * @author Mark Slee <mcslee@facebook.com>
 */
public class RunPerf
{
	/**
	 * @param args
	 */
	public static void main( String[] args ) throws Exception
	{
		String host = "localhost";
//		boolean framedInput = true;
//		boolean framedOutput = true;

		TSocket socket = new TSocket( host, 4002 );
		socket.setTimeout( 1000 );
		
		TTransport transport = socket;
//		transport = new TFramedTransport( transport, framedInput, framedOutput );

		TBinaryProtocol binaryProtocol = new TBinaryProtocol( transport );
		final Perf.Client client = new Perf.Client( binaryProtocol );

		transport.open();

		Assertion.check( client.add( 2, 3 ) == 5, "client.add( 2, 3 ) == 5" );
		
		Assertion.check( client.add( 3, 4 ) == 7, "client.add( 3, 4 ) == 7" );
		
		ArrayList<Integer> list = new ArrayList<Integer>();
		list.add( 1 );
		list.add( 2 );
		list.add( 3 );
		Assertion.check( client.sum( list ) == 6, "client.sum( list ) == 6" );
		
		list = new ArrayList<Integer>();
		list.add( 2 );
		list.add( 3 );
		list.add( 4 );
		Assertion.check( client.sum( list ) == 9, "client.sum( list ) == 9" );
		
		client.report( 23, "all ok" );
		
		client.report( 19, "stopping" );
		
		new PerfTest( "add", 10, 5 )
		{
			@Override
			public void run( long n ) throws TException
			{
				while (n-- > 0)
					client.add( 1000000000, 2000000000 );
			}
		}.run();
	
		int n = 1000;
		final ArrayList<Integer> values = new ArrayList<Integer>( n );
		for (int i = 0; i < n; i++)
			values.add( i );
		
		new PerfTest( "sum", 10, 5 )
		{
			@Override
			public void run( long n ) throws TException
			{
				while (n-- > 0)
					client.sum( values );
			}
		}.run();
	
		new PerfTest( "report", 10, 5 )
		{
			@Override
			public void run( long n ) throws TException
			{
				while (n-- > 0)
					client.report( 23,
						// 50 characters:
						"this message describes the specifics of a code 23." );
			}
		}.run();
		
		transport.close();
	}
}
