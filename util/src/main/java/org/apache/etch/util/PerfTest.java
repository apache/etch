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

package org.apache.etch.util;

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
	 * @param nThreads the number of threads to use for the test.
	 */
	public PerfTest( String descr, int runtime, int count, int nThreads )
	{
		if (runtime < 1)
			throw new IllegalArgumentException( "runtime < 1" );
		
		if (count < 1)
			throw new IllegalArgumentException( "count < 1" );
		
		if (nThreads < 1)
			throw new IllegalArgumentException( "nThreads < 1" );
		
		this.descr = descr;
		this.runtime = runtime;
		this.count = count;
		this.nThreads = nThreads;
	}
	
	/**
	 * Constructs the PerfTest.
	 * @param descr the description of the test.
	 * @param runtime the number of seconds to run each test cycle.
	 * @param count the number of test cycles to run.
	 */
	public PerfTest( String descr, int runtime, int count )
	{
		this( descr, runtime, count, 1 );
	}
	
	private final String descr;
	
	private final int runtime;
	
	private final int count;
	
	private final int nThreads;
	
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
			System.out.printf( "%s %d/%d\t%d\t%d\t%f\t%f\t%f\n",
				descr, i, count, nThreads, n, t, r, r*nThreads );
		}
		Collections.sort( list );
		System.out.printf( "%s min\t%f\n", descr, list.get( 0 )*nThreads );
		System.out.printf( "%s median\t%f\n", descr, list.get( count/2 )*nThreads );
		System.out.printf( "%s avg\t%f\n", descr, sum*nThreads/count );
		System.out.printf( "%s max\t%f\n", descr, list.get( count-1 )*nThreads );
    }
	
	final private double runOne( long n ) throws Exception
	{
	    long t0 = System.nanoTime();
	    
	    if (nThreads == 1)
	    	run( n );
	    else
	    	runThreads( n );
	    
	    long t1 = System.nanoTime();
	    return (t1 - t0) / 1000000000.0;
	}
	
	private void runThreads( final long n ) throws InterruptedException
	{
		Thread[] threads = new Thread[nThreads];
		
		for (int i = 0; i < nThreads; i++)
		{
			threads[i] = new Thread( new Runnable()
			{
				public void run()
				{
					try
					{
						PerfTest.this.run( n );
					}
					catch ( Exception e )
					{
						e.printStackTrace();
					}
				}
			} );
		}
		
		for (int i = 0; i < nThreads; i++)
			threads[i].start();
		
		for (int i = 0; i < nThreads; i++)
			threads[i].join();
	}

	/**
	 * Runs the test with the specific number of times.
	 * @param n
	 * @throws Exception
	 */
	abstract public void run( long n ) throws Exception;
}