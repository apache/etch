using System;
using System.Collections.Generic;
using System.Text;
using System.Threading;
using NUnit.Framework;

namespace Etch4.Util.Test
{
    public class TestHelp
    {

        public static Thread delayAndStart( int delay, Thread runnable )
	{
		Thread t = Delay( delay, runnable );
		t.Start();
		return t;
	}

    public static Thread Delay( int delay,  Thread runnable )
	{
		Thread t = new Thread( new ThreadStart(delegate()
		                                           {
		                                             try
				{
					Thread.Sleep( delay );
					runnable.Start();
				}
				catch ( Exception e )
				{
					Console.WriteLine(" The exception in TestHelp->delay is " + e.StackTrace);
				}  
		                                           } ));
		
			
		return t;
	}

    public static void AssertRelError(double expected, double actual, double error)
    {
        double relError = RelError(expected, actual);
        if (relError > error)
            Assert.Fail(String.Format("expected: {0} but was: {1} relative error: {2}", expected, actual, relError));
            
    }

    public static void AssertAbsError(double expected, double actual, double error)
    {
        double absError = AbsError(expected, actual);
        if (absError > error)
            Assert.Fail(String.Format("expected: {0} but was: {1} relative error: {2}", expected, actual, error));
    }

    public static double RelError(double expected, double actual)
    {
        return AbsError(expected, actual) / expected;
    }

    public static double AbsError(double expected, double actual)
    {
        return Math.Abs(expected - actual);
    }


    }
}
