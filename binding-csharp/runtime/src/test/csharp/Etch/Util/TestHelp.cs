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
					Console.WriteLine(" The exception in TestHelp->delay is " + e);
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
