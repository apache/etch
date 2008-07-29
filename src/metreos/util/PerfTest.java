package metreos.util;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * Framework for running performance tests.
 */
abstract public class PerfTest
{
	/**
	 * Constructs the PerfTest.
	 * @param descr the description of the test.
	 * @param runtime the number of seconds to run each test cycle.
	 * @param count the number of test cycles to run.
	 */
	public PerfTest( String descr, int runtime, int count )
	{
		this.descr = descr;
		this.runtime = runtime;
		this.count = count;
	}
	
	private final String descr;
	
	private final int runtime;
	
	private final int count;
	
	/**
	 * Runs the perf test.
	 * @throws Exception 
	 */
	final public void run() throws Exception
    {
		long oldn = 0;
		long n = 1;
		double t = 0;
		while (t < 1)
		{
			if (t > 0.1)
				System.out.printf( "%s: %d took %f, trying %d to get >= 1 second\n", descr, oldn, t, n );
			oldn = n;
			t = runOne( n );
			n *= 2;
		}
		n = oldn;
		
		int k = 2;
		n = (long) (k * n / t);
		System.out.printf( "%s: %d took %f, trying %d for %d seconds\n", descr, oldn, t, n, k );
		oldn = n;
		t = runOne( n );
		
		k = 4;
		n = (long) (k * n / t);
		System.out.printf( "%s: %d took %f, trying %d for %d seconds\n", descr, oldn, t, n, k );
		oldn = n;
		t = runOne( n );
		
		n = (long) (runtime * n / t);
		System.out.printf( "%s: %d took %f, using %d for %d seconds\n",
			descr, oldn, t, n, runtime );
		
		List<Double> list = new ArrayList<Double>( count );
		double sum = 0;
		for (int i = 1; i <= count; i++)
		{
			t = runOne( n );
			double r = n/t;
			list.add( r );
			sum += r;
			System.out.printf( "%s %d/%d\t%d\t%f\t%f\n",
				descr, i, count, n, t, r );
		}
		Collections.sort( list );
		System.out.printf( "%s min\t%f\n", descr, list.get( 0 ) );
		System.out.printf( "%s median\t%f\n", descr, list.get( count/2 ) );
		System.out.printf( "%s avg\t%f\n", descr, sum/count );
		System.out.printf( "%s max\t%f\n", descr, list.get( count-1 ) );
    }
	
	final private double runOne( long n ) throws Exception
	{
	    long t0 = System.nanoTime();
	    run( n );
	    long t1 = System.nanoTime();
	    return (t1 - t0) / 1000000000.0;
	}
	
	/**
	 * Runs the test with the specific number of times.
	 * @param n
	 * @throws Exception 
	 */
	abstract public void run( long n ) throws Exception;
}