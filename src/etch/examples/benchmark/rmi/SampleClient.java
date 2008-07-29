//---------------------------------------------------------------
// File: SampleClient.java
//---------------------------------------------------------------

import java.rmi.Naming;
import java.rmi.RMISecurityManager;

import metreos.util.PerfTest;

/**
 * Client for sample server for java rmi performance testing.
 */
public class SampleClient
{
	/**
	 * @param args
	 * @throws Exception 
	 */
	public static void main( String[] args ) throws Exception
	{
		System.setSecurityManager( new RMISecurityManager() );
		String url = "//localhost/SAMPLE-SERVER";
		final SampleServer server = (SampleServer) Naming.lookup( url );
		
		System.out.println( "Got remote object" );

		System.out.println( "add 1 + 2 = " + server.add( 1, 2 ) );
		System.out.println( "add 3 + 4 = " + server.add( 3, 4 ) );

		System.out.println( "sum { 1, 2, 3 } = " + server.sum( new int[] { 1, 2, 3 } ) );
		System.out.println( "sum { 2, 3, 4 } = " + server.sum( new int[] { 2, 3, 4 } ) );
		
		server.report( 23, "all ok" );
		server.report( 19, "stopping" );
		
		new PerfTest( "add", 10, 5 )
		{
			@Override
			public void run( long n ) throws Exception
			{
				while (n-- > 0)
					server.add( 1000000000, 2000000000 );
			}
		}.run();
	
		final int[] values = new int[1000];
		for (int i = 0; i < values.length; i++)
			values[i] = i;
		
		new PerfTest( "sum", 10, 5 )
		{
			@Override
			public void run( long n ) throws Exception
			{
				while (n-- > 0)
					server.sum( values );
			}
		}.run();
	
		new PerfTest( "report", 10, 5 )
		{
			@Override
			public void run( long n ) throws Exception
			{
				while (n-- > 0)
					server.report( 23,
						// 50 characters:
						"this message describes the specifics of a code 23." );
			}
		}.run();
		
		System.out.println( "done" );
	}
}
