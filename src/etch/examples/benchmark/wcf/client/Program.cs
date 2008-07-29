using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text;
using System.ServiceModel;
using Etch.Transport.Util;


namespace BenchmarkClient
{
    internal class Program
    {
        private static BenchmarkServiceClient _service;
        private static int[] _values;

        private static void Main(string[] args)
        {
            _service = new BenchmarkServiceClient();

            // make sure server is working

            Debug.Assert(_service.add(4, 5) == 9, "service.add( 4, 5 ) == 9");

            Debug.Assert(_service.add(2, 3) == 5, "service.add( 2, 3 ) == 5");

            Debug.Assert(_service.sum(new int[] { 1, 2, 3 }) == 6, "service.sum( new int[] {1, 2, 3} ) == 6");

            Debug.Assert(_service.sum(new int[] { 2, 4, 6 }) == 12, "service.sum( new int[] { 2, 4, 6 } ) == 12");

            _service.report(23, "all working");
            _service.report(19, "stopping");

            PerfTest pt1 = new PerfTest("add", 10, 5, RunAdd);
            pt1.Run();

            _values = new int[1000];
            for(int i = 0; i < _values.Length; i++)
                _values[i] = i;

            PerfTest pt2 = new PerfTest("sum", 10, 5, RunSum);
            pt2.Run();

            PerfTest pt3 = new PerfTest("report", 10, 5, RunReport);
            pt3.Run();
        }

        #region Run Delegates

        public static void RunAdd(long n)
        {
            while(n-- > 0)
                _service.add(1000000000, 2000000000);
        }

        public static void RunSum(long n)
        {
            while(n-- > 0)
                _service.sum(_values);
        }

        public static void RunReport(long n)
        {
            while(n-- > 0)
                _service.report(23,
                    // 50 characters:
                    "this message describes the specifics of a code 23.");
        }

        #endregion
    }
}