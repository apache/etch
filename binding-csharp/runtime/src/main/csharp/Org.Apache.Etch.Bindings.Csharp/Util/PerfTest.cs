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
using System.Collections.Generic;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// Framework for running performance tests.
    /// </summary>
    public class PerfTest
    {
        private readonly String _descr;
        private readonly int _runtime;
        private readonly int _count;
        private readonly RunTimes _r;

        /// <summary>
        /// Construct the perftest
        /// </summary>
        /// <param name="descr">description of the test</param>
        /// <param name="runtime">the number of seconds to run each test cycle</param>
        /// <param name="count">the number of test cycles to run</param>
        public PerfTest( String descr, int runtime, int count, RunTimes rt )
        {
            _descr = descr;
            _runtime = runtime;
            _count = count;
            _r = rt;
        }

        /// <summary>
        /// Runs the PerfTest
        /// </summary>
        public void Run( )
        {
            
            long oldn = 0;
            long n = 1;
            double t = 0;

            while (t < 1)
		    {
			    if (t > 0.1)
				    Console.WriteLine( "{0}: {1} took {2}, trying {3} to get >= 1 second\n", _descr, oldn, t, n );
			    oldn = n;
			    t = RunOne( n );
			    n *= 2;
		    }
		    n = oldn;

            int k = 2;
		    n = (long) (k * n / t);
		    Console.WriteLine( "{0}: {1} took {2}, trying {3} for {4} seconds\n", _descr, oldn, t, n, k );
		    oldn = n;
		    t = RunOne( n );

            k = 4;
		    n = (long) (k * n / t);
		    Console.WriteLine( "{0}: {1} took {2}, trying {3} for {4} seconds\n", _descr, oldn, t, n, k );
		    oldn = n;
		    t = RunOne( n );
    		
		    n = (long) (_runtime * n / t);
            Console.WriteLine( "{0}: {1} took {2}, using {3} for {4} seconds\n",
			    _descr, oldn, t, n, _runtime );

            List<double> list = new List<double>( _count );
            double sum = 0;
		    for (int i = 1; i <= _count; i++)
		    {
			    t = RunOne( n );
			    double r = n/t;
			    list.Add( r );
			    sum += r;
			    Console.WriteLine( "{0} {1}/{2}\t{3}\t{4}\t{5}\n",
				    _descr, i, _count, n, t, r );
		    }

            list.Sort();
            Console.WriteLine( "{0} min\t{1}\n", _descr, list[ 0 ] );
            Console.WriteLine( "{0} median\t{1}\n", _descr, list[ _count/2 ] );
		    Console.WriteLine( "{0} avg\t{1}\n", _descr, sum/_count );
		    Console.WriteLine( "{0} max\t{1}\n", _descr, list[ _count-1 ] );
        }

        private double RunOne( long n )
        {
            long t0 = HPTimer.Now();
            _r( n );
            return HPTimer.SecondsSince(t0);
        }

        /// <summary>
        /// Runs the test with the specific number of times.
        /// </summary>
        /// <param name="n"></param>
        public delegate void RunTimes( long n );
    }
}
