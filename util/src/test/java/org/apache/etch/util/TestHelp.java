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

import static org.junit.Assert.fail;

import org.junit.Test;

/**
 * Help for junit tests.
 */
public class TestHelp
{
	/** @throws Exception */
	@Test
	public void testRelError1() throws Exception
	{
		assertRelError( 10, 9, .11 );
		assertRelError( 10, 11, .14 );
		assertRelError( 20, 19, .07 );
		assertRelError( 19, 23, .22 );
	}
	
	/** @throws Exception */
	@Test( expected = AssertionError.class )
	public void testRelError2() throws Exception
	{
		assertRelError( 9, 8, .1 );
	}
	
	/** @throws Exception */
	@Test( expected = AssertionError.class )
	public void testRelError3() throws Exception
	{
		assertRelError( 9, 10, .1 );
	}
	
	/** @throws Exception */
	@Test
	public void testAbsError1() throws Exception
	{
		assertAbsError( 11, 15, 4 );
		assertAbsError( 15, 10, 5 );
		assertAbsError( 5, 3, 2 );
		assertAbsError( 4, 7, 3 );
	}
	
	/** @throws Exception */
	@Test( expected = AssertionError.class )
	public void testAbsError2() throws Exception
	{
		assertAbsError( 11, 15, 3 );
	}
	
	/** @throws Exception */
	@Test( expected = AssertionError.class )
	public void testAbsError3() throws Exception
	{
		assertAbsError( 19, 15, 3 );
	}
	
	/**
	 * Creates a thread to delay running something.
	 * @param delay
	 * @param runnable
	 * @return the thread which is not started.
	 */
	public static Thread delay( final int delay, final DelayRunnable runnable )
	{
		Thread t = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					Thread.sleep( delay );
					runnable.run();
				}
				catch ( Exception e )
				{
					e.printStackTrace();
				}
			}
		} );
		return t;
	}

	/**
	 * Creates a thread to delay running something.
	 * @param delay
	 * @param runnable
	 * @return the thread which is started.
	 */
	public static Thread delayAndStart( final int delay, final DelayRunnable runnable )
	{
		Thread t = delay( delay, runnable );
		t.start();
		return t;
	}
	
	/**
	 * Interface to implement when using delay.
	 */
	public interface DelayRunnable
	{
		/**
		 * @throws Exception
		 */
		public void run() throws Exception;
	}

	/**
	 * Compare doubles with relative error.
	 * @param expected
	 * @param actual
	 * @param maxRelError
	 * @see #relError(double, double)
	 */
	public static void assertRelError( double expected, double actual, double maxRelError )
	{
		double relError = relError( expected, actual );
		if (relError > maxRelError)
			fail( String.format( "expected: <%f> but was: <%f> relative error: <%f>", expected, actual, relError ) );
	}
	
	/**
	 * Compare doubles with absolute error.
	 * @param expected
	 * @param actual
	 * @param maxAbsError
	 * @see #absError(double, double)
	 */
	public static void assertAbsError( double expected, double actual, double maxAbsError )
	{
		double absError = absError( expected, actual );
		if (absError > maxAbsError)
			fail( String.format( "expected: <%f> but was: <%f> absolute error: <%f>", expected, actual, absError ) );
	}
	
	/**
	 * Computes the relative error between actual and expected.
	 * @param expected
	 * @param actual
	 * @return the relative error absError( expected, actual ) / expected.
	 * @see #absError(double, double)
	 */
	public static double relError( double expected, double actual )
	{
		return absError( expected, actual ) / expected;
	}
	
	/**
	 * Computes the absolute error between actual and expected.
	 * @param expected
	 * @param actual
	 * @return the absolute error Math.abs( expected - actual ).
	 */
	public static double absError( double expected, double actual )
	{
		return Math.abs( expected - actual );
	}
}
