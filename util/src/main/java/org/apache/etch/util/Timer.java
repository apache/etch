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

/**
 * Timer enables making high precision interval tests. In the usual
 * scenario, some code is going to be bracketed by calls to
 * initialize a timer and later get the elapsed time since the init.
 * 
 * Trying to use the system clock (System.currentTimeMillis) is
 * problematic because periodic system time adjustments (by ntp,
 * windows time, etc.) will invalidate any interval testing based
 * on the system clock. Also, the system clock resolution is not
 * all that good (15 ms typically on a windows box).
 * 
 * Timer tmr = new Timer().init();
 * foo();
 * long ns = tmr.elapsedNanos();
 * System.out.println( "foo() took "+ns+" nanos" );
 * 
 * Timer keeps state, initialized by init(), and used by elapsedBlah()
 * and thus is thread safe as long as calls to init() are serialized.
 * 
 * For lower cost timing needs, Timer supports a set of static interfaces.
 * 
 * long t0 = Timer.getStartTime();
 * foo();
 * long ns = Timer.getNanosSince( t0 );
 * System.out.println( "foo() took "+ns+" nanos" );
 * 
 * Note: reading the high precision clock (which all these methods
 * ultimately do) takes about 1,487 nanoseconds on Windows XP Pro
 * running on a Dell Precision 370 workstation using jdk 1.5.0_04.
 * By comparison, a method call takes about 1.97 nanoseconds. Also,
 * on the above platform, the resolution of the timer is 1,396 ns.
 * Could be that the resolution is smaller, but we'll never know it
 * because just getting the value from the OS is the dominant factor.
 * 
 * @see #getNanos()
 * @see #getNanosSince(long)
 */
public class Timer
{
	/**
	 * Constructs the Timer. The timer is not started.
	 * @see #init()
	 */
	public Timer()
	{
		// nothing to do.
	}
	
	/**
	 * Initializes the startTime of this timer.
	 * @return returns this timer.
	 */
	public Timer init()
	{
		startTime = getNanos();
		return this;
	}
	
	/**
	 * @return the elapsed time in nanos since the last init.
	 */
	public long elapsedNanos()
	{
		if (startTime == Long.MIN_VALUE)
			throw new UnsupportedOperationException( "Timer not started" );
		return getNanosSince( startTime );
	}
	
	/**
	 * @return the elapsed time in micros since the last init.
	 */
	public long elapsedMicros()
	{
		return elapsedNanos() / NANOS_PER_MICRO;
	}
	
	/**
	 * @return the elapsed time in millis since the last init.
	 */
	public long elapsedMillis()
	{
		return elapsedNanos() / NANOS_PER_MILLI;
	}
	
	/**
	 * @return the elapsed time in seconds since the last init.
	 */
	public double elapsedSeconds()
	{
		return elapsedNanos() / NANOS_PER_SECOND;
	}
	
	private long startTime = Long.MIN_VALUE;
	
	/**
	 * Number of nanoseconds per microsecond.
	 */
	public final static long NANOS_PER_MICRO = 1000;
	
	/**
	 * Number of nanoseconds per milliseconds.
	 */
	public final static long NANOS_PER_MILLI = 1000000;
	
	/**
	 * Number of nanoseconds per second.
	 */
	public final static double NANOS_PER_SECOND = 1000000000.0;

	//////////////////////
	// STATIC INTERFACE //
	//////////////////////
	
	/**
	 * @return the starting point of a timing test in nanos.
	 */
	public static long getNanos()
	{
		return System.nanoTime();
	}
	
	/**
	 * Don't use this method for newly built timing. This is only
	 * for easy backwards compatible (for delta time purposes) replacement
	 * of System.currentTimeMillis().
	 * 
	 * @return the starting point of a timing test in millis.
	 * 
	 * @see System#currentTimeMillis()
	 */
	public static long currentTimeMillis()
	{
		return getNanos() / NANOS_PER_MILLI;
	}
	
	/**
	 * @param startTime a value returned by getStartTime.
	 * 
	 * @return the number of nanos which have passed since startTime.
	 * 
	 * @see #getNanos()
	 */
	public static long getNanosSince( long startTime )
	{
		return System.nanoTime() - startTime;
	}
	
	/**
	 * @param startTime a value returned by getStartTime.
	 * 
	 * @return the number of micros which have passed since startTime.
	 * 
	 * @see #getNanos()
	 */
	public static long getMicrosSince( long startTime )
	{
		return getNanosSince( startTime ) / NANOS_PER_MICRO;
	}
	
	/**
	 * @param startTime a value returned by getStartTime.
	 * 
	 * @return the number of millis which have passed since startTime.
	 * 
	 * @see #getNanos()
	 */
	public static long getMillisSince( long startTime )
	{
		return getNanosSince( startTime ) / NANOS_PER_MILLI;
	}

	/**
	 * @param startTime a value returned by getStartTime.
	 * 
	 * @return the number of seconds which have passed since startTime.
	 * 
	 * @see #getNanos()
	 */
	public static double getSecondsSince( long startTime )
	{
		return getNanosSince( startTime ) / NANOS_PER_SECOND;
	}
	
	/**
	 * @return returns the minimum value of many invocations of
	 * getNanosSince( getStartTime() ).
	 */
	public static long getResolution()
	{
		long r = Long.MAX_VALUE;
		for (int i = 0; i < 100000; i++)
		{
			r = Math.min( r, getNanosSince( getNanos() ) );
		}
		return r;
	}
	
	/**
	 * @param s the number of seconds
	 * @return a nice text description of the amount of time
	 */
	public static String howLong( int s )
	{
		if (s < 60)
			return Integer.toString( s ) + "s";
		
		int m = s / 60;
		if (m < 60)
			return Integer.toString( m ) + "m "+s%60+"s";
		
		int h = m / 60;
		if (h < 24)
			return Integer.toString( h ) + "h "+m%60+"m";
		
		return Integer.toString( h / 24 ) + "d "+h%24+"h";
	}
}
