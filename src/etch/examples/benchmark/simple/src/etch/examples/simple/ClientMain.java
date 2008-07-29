package etch.examples.simple;

import metreos.core.io.TcpConnection;
import metreos.util.Log;
import etch.bindings.java.transport.*;

public class ClientMain 
{
    static StubSimpleClient sfc;

	private static void testVoid(SimpleServer rfs, int n, int iteration)
	{
		long t0 = System.nanoTime();
		
		for (int i = 0; i < n; i++)
			rfs.report( 23, "blah" );
		
		long t1 = System.nanoTime();
		double t = (t1-t0) / 1000000000.0;
		double r = n / t;
		Log.report( "testVoid", "iteration", iteration, "time", t, "count", n, "rate", r );
	}
    
	private static void testAdd(SimpleServer rfs, int n, int iteration)
	{
		long t0 = System.nanoTime();
		
		for (int i = 0; i < n; i++)
			rfs.add(i, i);
		
		long t1 = System.nanoTime();
		double t = (t1-t0) / 1000000000.0;
		double r = n / t;
		Log.report( "testAdd", "iteration", iteration, "time", t, "count", n, "rate", r );
	}

	private static void testSum(SimpleServer rfs, int n, int iteration, int size)
	{
		long t0 = System.nanoTime();
		
		int[] vals = new int[size];
		for (int i=0; i<size; i++)
			vals[i] = i;
		
		for (int i = 0; i < n; i++)
			rfs.sum(vals);
		
		long t1 = System.nanoTime();
		double t = (t1-t0) / 1000000000.0;
		double r = n / t;
		Log.report( "testSum", "iteration", iteration, "time", t, "count", n, "rate", r );
	}
	
    public static void main( String[] args ) throws Exception
    {
		Log.addSink( null );
		Log.report( "Simple Benchmark Test" );
    	
        ValueFactorySimple vf = new ValueFactorySimple();
        
        DefaultDeliveryService2<StubSimpleClient> dds 
            = new DefaultDeliveryService2<StubSimpleClient>()
        {
            @Override
            protected StubSimpleClient newStub( MailboxMessageSource src )
            {
                return sfc;
            }

            @Override
            public String toString()
            {
                return "ClientDeliveryServer";
            }
        };

        TcpConnection c = new TcpConnection( null, "", 4001, 0 );
        
        DefaultDeliveryService2 svc = 
            DefaultDeliveryService2.setUp(dds, c, vf );
        
        final SimpleServer rfs = new RemoteSimpleServer( svc );
        
        sfc = new StubSimpleClient( new SimpleClientImpl(), null, null );
        
        c.start();
        c.waitUp( 4000 );

		int n = 25000;
		for (int i = 0; i < 5; i++)
			testVoid( rfs, n, i );
        
		n = 14000;
		for (int i = 0; i < 5; i++)
			testAdd( rfs, n, i );
		
		n = 8000; 
		for (int i = 0; i < 5; i++)
			testSum( rfs, n, i, 4000 );
		
        System.out.println("Press enter to quit.");
        System.in.read();
        System.out.println("Exiting.");

		((RemoteSimpleServer) rfs)._shutdownOutput();        
        c.stop();
        c.waitDown(4000);
    }
}
