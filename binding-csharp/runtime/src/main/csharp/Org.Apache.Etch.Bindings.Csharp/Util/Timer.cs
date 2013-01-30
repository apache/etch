// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Timer enables making high precision interval tests. In the usual
    /// scenario, some code is going to be bracketed by calls to
    /// initialize a timer and later get the elapsed time since the init.
    /// 
    /// Trying to use the system clock (System.currentTimeMillis) is
    /// problematic because periodic system time adjustments (by ntp,
    /// windows time, etc.) will invalidate any interval testing based
    /// on the system clock. Also, the system clock resolution is not
    /// all that good (15 ms typically on a windows box).
    /// 
    /// Timer tmr = new Timer().init();
    /// foo();
    /// long ns = tmr.elapsedNanos();
    /// System.out.println( "foo() took "+ns+" nanos" );
    /// 
    /// Timer keeps state, initialized by init(), and used by elapsedBlah()
    /// and thus is thread safe as long as calls to init() are serialized.
    /// 
    /// For lower cost timing needs, Timer supports a set of static interfaces.
    /// 
    /// long t0 = Timer.getStartTime();
    /// foo();
    /// long ns = Timer.getNanosSince( t0 );
    /// System.out.println( "foo() took "+ns+" nanos" );
    /// 
    /// Note: reading the high precision clock (which all these methods
    /// ultimately do) takes about 1,487 nanoseconds on Windows XP Pro
    /// running on a Dell Precision 370 workstation using jdk 1.5.0_04.
    /// By comparison, a method call takes about 1.97 nanoseconds. Also,
    /// on the above platform, the resolution of the timer is 1,396 ns.
    /// Could be that the resolution is smaller, but we'll never know it
    /// because just getting the value from the OS is the dominant factor.
    /// </summary>

    public class Timer
    {
        /// <summary>
        /// Constructs the Timer. The timer is not started.
        /// </summary>
        public Timer()
        {
            // nothing to do.
        }

        /// <summary>
        /// Initializes the startTime of this timer.
        /// </summary>
        /// <returns>returns this timer.</returns>
        public Timer Init()
        {
            startTime = Nanos;
            return this;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the elapsed time in nanos since the last init.</returns>
        public long ElapsedNanos()
        {
            if ( startTime == long.MinValue)
                throw new Exception( "Timer not started" );
            return GetNanosSince( startTime );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the elapsed time in micros since the last init.</returns>
        public long ElapsedMicros()
        {
            return ElapsedNanos() / NANOS_PER_MICRO;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the elapsed time in millis since the last init.</returns>
        public long ElapsedMillis()
        {
            return ElapsedNanos() / NANOS_PER_MILLI;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the elapsed time in seconds since the last init.</returns>
        public double ElapsedSeconds()
        {
            return ElapsedNanos() / NANOS_PER_SECOND;
        }

        private long startTime = long.MinValue;

        /// <summary>
        /// Number of nanoseconds per microsecond.
        /// </summary>
        public const long NANOS_PER_MICRO = 1000;

        /// <summary>
        /// Number of nanoseconds per milliseconds.
        /// </summary>
        public const long NANOS_PER_MILLI = 1000000;
        
        public const long TICKS_PER_MILLI = 10000; // 100 ns per tick
        
        public const long NANOS_PER_TICK = 100; // 100 ns per tick

        /// <summary>
        /// Number of nanoseconds per second.
        /// </summary>
        public const double NANOS_PER_SECOND = 1000000000.0;

        //////////////////////
        // STATIC INTERFACE //
        //////////////////////

        public static long Nanos
        {
            get
            {
                return DateTime.Now.Ticks * NANOS_PER_TICK;
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>the starting point of a timing test in millis.</returns>
        public static long currentTimeMillis()
        {
            return DateTime.Now.Ticks / TICKS_PER_MILLI;
        }
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="startTime">a value returned by getStartTime.</param>
        /// <returns>the number of nanos which have passed since startTime.</returns>
        /// <see cref="Nanos"/>
        public static long GetNanosSince( long startTime )
        {
            return Nanos - startTime;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="startTime">a value returned by getStartTime.</param>
        /// <returns>the number of micros which have passed since startTime.</returns>
        /// <see cref="Nanos"/>
        public static long GetMicrosSince( long startTime )
        {
            return GetNanosSince( startTime ) / NANOS_PER_MICRO;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="startTime">a value returned by getStartTime.</param>
        /// <returns>the number of millis which have passed since startTime.</returns>
        /// <see cref="Nanos"/>
        public static long GetMillisSince( long startTime )
        {
            return GetNanosSince( startTime ) / NANOS_PER_MILLI;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="startTime">a value returned by getStartTime.</param>
        /// <returns>the number of seconds which have passed since startTime.</returns>
        /// <see cref="Nanos"/>
        public static double GetSecondsSince( long startTime )
        {
            return GetNanosSince( startTime ) / NANOS_PER_SECOND;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>returns the minimum value of many invocations of
        /// getNanosSince( getStartTime() ).</returns>
        public static long GetResolution()
        {
            long r = long.MaxValue;
            for ( int i = 0; i < 100000; i++ )
            {
                r = Math.Min( r, GetNanosSince( Nanos ) );
            }
            return r;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="s">the number of seconds</param>
        /// <returns>a nice text description of the amount of time</returns>
        public static String HowLong( int s )
        {
            if ( s < 60 )
                return s.ToString() + "s";

            int m = s / 60;
            if ( m < 60 )
                return m.ToString() + "m" + ( s%60 ) + "s";

            int h = m / 60;
		    if (h < 24)
                return h.ToString() + "h " + ( m%60 ) + "m";

            int h_ = h / 24;
            return h_.ToString() + "d " + ( h%24 ) + "h";
        }
    }
}
