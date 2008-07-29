using System;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Support;
using Etch.Transport;

namespace etch.examples.benchmark.etch
{
    public class ImplPerfServer : PerfServer
    {

        #region PerfServer Members

        public int? add( int? x, int? y )
        {
            return x + y;
        }

        public int? sum( int[] values )
        {
            int sum = 0;
		    foreach (int value in values)
			    sum += value;
		    return sum;
        }

        public void report( int? code, string msg )
        {
            // do nothing
        }

        #endregion
    }
}
