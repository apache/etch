using System;
using System.Collections.Generic;
using System.Text;
using Etch.Transport;

namespace Etch.Util
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
            long t0 = Etch.Util.HPTimer.Now();
            _r( n );
            long t1 = Etch.Util.HPTimer.NsSince( t0 );
            return ( t1 )/ 1000000000.0;
        }

        /// <summary>
        /// Runs the test with the specific number of times.
        /// </summary>
        /// <param name="n"></param>
        public delegate void RunTimes( long n );
    }
}
