using System;
using System.Runtime.InteropServices;
using Etch.Transport;

namespace Etch.Util
{
	public abstract class HPTimer
	{
		[DllImport("kernel32.dll")]
		private static extern bool QueryPerformanceCounter(out long x);

		[DllImport("kernel32.dll")]
		private static extern bool QueryPerformanceFrequency(out long x);

		static HPTimer()
		{
			long freq = 0;

			if (!QueryPerformanceFrequency(out freq))
				throw new Exception( "QueryPerformanceFrequency not supported" );
			
			// freq is ticks / second.
			// we wanna compute seconds from ticks.
			// the easy computation is seconds = ticks / freq
			// to Get ns, we compute ns = ticks * 1,000,000,000 / freq
			// the problem is ticks * 1,000,000,000 will likely overflow.
			// what we want to do is precompute nsPerTick = 1,000,000,000 / freq
			// and then compute ns = ticks * nsPerTick.

			//Console.WriteLine( "HPTimer: freq = "+freq );
			nsPerTick = ((double) NS_PER_SECOND)/freq;
			//Console.WriteLine( "HPTimer: nsPerTick = "+nsPerTick );
		}

		private const long NS_PER_MILLISECOND = 1000000;
        private const long NS_PER_SECOND = 1000 * NS_PER_MILLISECOND;
        private const long NS_PER_MINUTE = 60 * NS_PER_SECOND;

		/// <summary>
		/// Returns the current high precision timer value in nanos.
		/// </summary>
		/// <returns></returns>
		public static long Now()
		{
			long x = 0;
			
			if (!QueryPerformanceCounter(out x))
				throw new Exception( "QueryPerformanceCounter not supported" );

			return (long) (x * nsPerTick);
		}

		/// <summary>
		/// Returns the difference in nanos between the current timer value and
		/// a timer value previously returned by Now.
		/// </summary>
		/// <param name="y"></param>
		/// <returns></returns>
		public static long NsSince( long y )
		{
			return Now() - y;
		}

		/// <summary>
		/// Returns the difference in millis between the current timer value and
		/// a timer value previously returned by Now.
		/// </summary>
		/// <param name="y"></param>
		/// <returns></returns>
		public static long MillisSince( long y )
		{
			long x = Now();
			return (x - y) / NS_PER_MILLISECOND;
		}

		/// <summary>
		/// Returns the difference in seconds between the current timer value and
		/// a timer value previously returned by Now.
		/// </summary>
		/// <param name="y"></param>
		/// <returns></returns>
		public static long SecondsSince( long y )
		{
			long x = Now();
			return (x - y) / NS_PER_SECOND;
		}

		/// <summary>
		/// Returns the number of high precision clock ticks per nano.
		/// </summary>
		/// <returns></returns>
		public static double NsPerTick()
		{
			return nsPerTick;
		}

        /// <summary>Adds the specified amount of time to the base time specified</summary>
        /// <param name="baseTime">Time to add to</param>
        /// <param name="minutes">Number of minutes to add</param>
        /// <param name="seconds">Number of seconds to add</param>
        /// <param name="millis">Number of milliseconds to add</param>
        /// <returns>The total value</returns>
        public static long AddTime(long baseTime, int minutes, int seconds, int millis)
        {
            baseTime += minutes * NS_PER_MINUTE;
            baseTime += seconds * NS_PER_SECOND;
            baseTime += millis * NS_PER_MILLISECOND;            
            return baseTime;
        }

		private static double nsPerTick;
	}
}
