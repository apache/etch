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
using System.Diagnostics;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
	public abstract class HPTimer
	{
		public const long NS_PER_MICROSECOND = 1000;
		public const long NS_PER_MILLISECOND = 1000 * NS_PER_MICROSECOND;
		public const long NS_PER_SECOND = 1000 * NS_PER_MILLISECOND;

		private static Stopwatch stopwatch;

		static HPTimer()
		{
			stopwatch = new Stopwatch();
			stopwatch.Start();

			nsPerTick = NS_PER_SECOND / Stopwatch.Frequency;
		}

		/// <summary>
		/// Returns the current high precision timer value in nanos.
		/// </summary>
		/// <returns></returns>
		public static long Now()
		{
			return stopwatch.ElapsedTicks * nsPerTick;
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
			return NsSince(y) / NS_PER_MILLISECOND;
		}

		/// <summary>
		/// Returns the difference in seconds between the current timer value and
		/// a timer value previously returned by Now.
		/// </summary>
		/// <param name="y"></param>
		/// <returns></returns>
		public static double SecondsSince( long y )
		{
			return NsSince(y) / NS_PER_SECOND;
		}

		/// <summary>
		/// Returns the number of high precision clock ticks per nano.
		/// </summary>
		/// <returns></returns>
		public static long NsPerTick()
		{
			return nsPerTick;
		}

		private static long nsPerTick;
	}
}
