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

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    internal delegate void run();
    
    [TestFixture]
    public class TestCircularQueue
    {

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write("TestCircularQueue");
        }

    [Test]
    [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
    public void construct1() 
    {
        new CircularQueue<int?>( 0 );
    }

    [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
	public void construct2() 
	{
		new CircularQueue<int?>( -1 );
	}
	
	
	[Test]
	public void construct3() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>( 1 );
		Assert.AreEqual( 1, queue.Size() );
	}
	
	
	[Test]
	public void construct4() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>( 2 );
		Assert.AreEqual( 2, queue.Size() );
	}
	
	
	[Test]
	public void construct5() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>();
		Assert.AreEqual( 10, queue.Size() );
	}
	
	
	[Test]
	public void putget1() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>();
		
		Assert.AreEqual( 0, queue.Count() );
		Assert.IsTrue( queue.IsEmpty() );
		Assert.IsFalse( queue.IsFull() );
		
		for (int i = 0; i < 10000; i++)
		{
			queue.Put( i );
			Assert.AreEqual( 1, queue.Count() );
			Assert.IsFalse( queue.IsEmpty() );
			
			Assert.AreEqual( i, queue.Get() );
			Assert.AreEqual( 0, queue.Count() );
			Assert.IsTrue( queue.IsEmpty() );
		}
	}
	
	
	[Test]
	public void putget2() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>();
		
		Assert.IsNull( queue.Get( 1 ) );
		Assert.IsNull( queue.Get( -1 ) );
		Assert.IsNull( queue.Get( 10 ) );
		
		queue.Put( 9 );
		Assert.AreEqual( 9, queue.Get( 1 ) );
		
		queue.Put( 9 );
		Assert.AreEqual( 9, queue.Get( -1 ) );
		
		queue.Put( 9 );
		Assert.AreEqual( 9, queue.Get( 10 ) );
		
		Assert.IsNull( queue.Get( 1 ) );
		Assert.IsNull( queue.Get( -1 ) );
		Assert.IsNull( queue.Get( 10 ) );
		
	/*	long t0;
		double t;
		
	
	    t0 = HPTimer.Now();
	    Assert.IsNull(queue.Get(10));
        t = ( HPTimer.Now() - t0) / 1000000.0;
		assertRelError( 10, t, 1 );
		
		
        t0 = HPTimer.Now();
	    Assert.IsNull(queue.Get(20));
        t = (HPTimer.Now() - t0) / 1000000.0;
		assertRelError( 20, t, 1 );
		
	
        t0 = HPTimer.Now();
		Assert.IsNull( queue.Get( 30 ) );
        t = (HPTimer.Now() - t0) / 1000000.0;
		assertRelError( 30, t, 1 ); */
	}
	
	
	[Test]
	public void putget3() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>();
		
		Assert.IsTrue( queue.Put( 1 ) );
		Assert.IsTrue( queue.Put( 2 ) );
		Assert.IsTrue( queue.Put( 3 ) );
		Assert.AreEqual( 1, queue.Get() );
		Assert.AreEqual( 2, queue.Get() );
		Assert.AreEqual( 3, queue.Get() );
		
		Assert.IsTrue( queue.Put( 1, 0 ) );
		Assert.IsTrue( queue.Put( 2, 0 ) );
		Assert.IsTrue( queue.Put( 3, 0 ) );
		Assert.AreEqual( 1, queue.Get( 0 ) );
		Assert.AreEqual( 2, queue.Get( 0 ) );
		Assert.AreEqual( 3, queue.Get( 0 ) );
		
		Assert.IsTrue( queue.Put( 1, -1 ) );
		Assert.IsTrue( queue.Put( 2, -1 ) );
		Assert.IsTrue( queue.Put( 3, -1 ) );
		Assert.AreEqual( 1, queue.Get( -1 ) );
		Assert.AreEqual( 2, queue.Get( -1 ) );
		Assert.AreEqual( 3, queue.Get( -1 ) );
		
		Assert.IsTrue( queue.Put( 1, 1 ) );
		Assert.IsTrue( queue.Put( 2, 1 ) );
		Assert.IsTrue( queue.Put( 3, 1 ) );
		Assert.AreEqual( 1, queue.Get( 1 ) );
		Assert.AreEqual( 2, queue.Get( 1 ) );
		Assert.AreEqual( 3, queue.Get( 1 ) );
	}
	
	
	[Test]
	public void get() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>( 1 );
		
	//	System.nanoTime();
        HPTimer.Now();
		Assert.IsNull( queue.Get( 10 ) );
		assertRelError( 1, 1, 1 );

	    foreach (int i in new int[] { 10, 20, 30, 50, 80, 130, 210, 340, 550, 890, 1440 })	   
		{
		//	Console.WriteLine( "get delay = {0}\n", i );
            long t0 = HPTimer.Now();
			Assert.IsNull( queue.Get( i ) );
      /*      double t = (HPTimer.Now() - t0) / 1000000.0;
			assertRelError( i, t, 2 ); */
		}
	}
	
	
	[Test]
        [ExpectedException(typeof(ArgumentNullException))]
	public void put() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>();
		queue.Put( null );
	}
	
	
	[Test]
	public void full() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>( 1 );
		
		Assert.IsFalse( queue.IsFull() );
		Assert.IsTrue( queue.Put( 0 ) );
		Assert.IsTrue( queue.IsFull() );
		Assert.AreEqual( 0, queue.Get() );
		Assert.IsFalse( queue.IsFull() );
		
		Assert.IsTrue( queue.Put( 0 ) );
		Assert.IsFalse( queue.Put( 0, -1 ) );
		Assert.IsFalse( queue.Put( 0, 1 ) );
		Assert.IsFalse( queue.Put( 0, 10 ) );
	}
	
	
	[Test]
	public void close1() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>();
		
		Assert.IsFalse( queue.IsClosed() );
		queue.Close();
		Assert.IsTrue( queue.IsClosed() );
		
		Assert.IsNull( queue.Get() );
		Assert.IsNull( queue.Get( -1 ) );
		Assert.IsNull( queue.Get( 0 ) );
		Assert.IsNull( queue.Get( 1 ) );
		
		queue.Close();
		Assert.IsTrue( queue.IsClosed() );
	}
	
	
	[Test]
	public void close2() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>();

		Assert.IsFalse( queue.IsClosed() );
		queue.Close();
		Assert.IsTrue( queue.IsClosed() );
		
		Assert.IsFalse( queue.Put( 0 ) );
		Assert.IsFalse( queue.Put( 0, -1 ) );
		Assert.IsFalse( queue.Put( 0, 0 ) );
		Assert.IsFalse( queue.Put( 0, 1 ) );
		
		queue.Close();
		Assert.IsTrue( queue.IsClosed() );
	}
	
	
	[Test]
	public void close3() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>();
		queue.Put( 1 );
		queue.Put( 2 );
		queue.Put( 3 );
		queue.Close();
		Assert.AreEqual( 1, queue.Get() );
		Assert.AreEqual( 2, queue.Get() );
		Assert.AreEqual( 3, queue.Get() );
		Assert.IsNull( queue.Get() );
	}
	
	
	[Test]
	public void delay1() 
	{
		CircularQueue<int?> queue = new CircularQueue<int?>();
		
		Delay( 10, delegate() {queue.Put( 99 ); } );
		Assert.AreEqual( 99, queue.Get() );
		
		Delay( 10, delegate() { queue.Close(); } );
		Assert.IsNull( queue.Get() ); 
	}
	
	
	[Test]
	public void delay2() 
	{
		 CircularQueue<int?> queue = new CircularQueue<int?>( 1 );
		
		Assert.IsTrue( queue.Put( 1 ) );
		Delay( 10, delegate() { Assert.AreEqual( 1, queue.Get() ); } );
		Assert.IsTrue( queue.Put( 2 ) );
		Assert.AreEqual( 2, queue.Get() );
		
		Assert.IsTrue( queue.Put( 1 ) );
	    Delay(10, delegate() { queue.Close(); });
		Assert.IsFalse( queue.Put( 2 ) );
		Assert.AreEqual( 1, queue.Get() );
		Assert.IsNull( queue.Get() );
	}
	
	
    [Test]
    public void stress1() 
    {
         CircularQueue<int?> queue = new CircularQueue<int?>( 1 );
         int n = 10000;

         Thread t = new Thread(
             delegate()
             {
                for (int i = 0; i < n; i++)
                {
                    try
                    {
                        queue.Put( i );
                    }
                    catch ( ThreadInterruptedException e )
                    {
                        Console.WriteLine(e);
                    }
                }  
             }
        );

        t.Start();

        for (int i = 0; i < n; i++)
            Assert.AreEqual(i, queue.Get());
    }

    [Test]
    public void stress2()
    {
        CircularQueue<int?> queue = new CircularQueue<int?>(1);
        int n = 1000;

        Thread t = new Thread(
            delegate()
            {
                for (int i = 0; i < n; i++)
                {
                    try
                    {
                        Thread.Sleep(5);
                        queue.Put(i);
                    }
                    catch (ThreadInterruptedException e)
                    {
                        Console.WriteLine(e);
                    }
                }
            }
       );

        t.Start();

        for (int i = 0; i < n; i++)
            Assert.AreEqual(i, queue.Get());
    }


    [Test]
    public void stress3()
    {

        CircularQueue<int?> queue = new CircularQueue<int?>(1);
        int n = 1000;

        Thread t = new Thread(
            delegate()
            {
                for (int i = 0; i < n; i++)
                {
                    try
                    {

                        queue.Put(i);
                    }
                    catch (ThreadInterruptedException e)
                    {
                        Console.WriteLine(e);
                    }
                }
            }
       );

        t.Start();



        for (int i = 0; i < n; i++)
        {
            Thread.Sleep(5);
            Assert.AreEqual(i, queue.Get());
        }
    }

    [Test]
    public void stress4() 
    {
         CircularQueue<int?> queue = new CircularQueue<int?>();
		
        // we will setup two threads waiting to get and
        // then in a single synchronized step put two
        // items in the queue. the first thread will be
        // woken to get, and once done the second thread
        // should be woken by the first.
		
        Thread t1 = new Thread( 
            delegate()
            {			
                try
                {
                    Assert.IsNotNull( queue.Get() );
                }
                catch ( ThreadInterruptedException e )
                {
                    Console.WriteLine(e);
                }
            }
        );

        Thread t2 = new Thread(
            delegate()
            {
                try
                {
                    Assert.IsNotNull(queue.Get());
                }
                catch (ThreadInterruptedException e)
                {
                    Console.WriteLine(e);
                }
            }
        );

        t1.Start();
        t2.Start();

        // wait until both threads are waiting on queue...

        Thread.Sleep(100);

        lock (queue)
        {
            queue.Put(1);
            queue.Put(2);
        }

        harvest(t1);
        harvest(t2);
    }


    [Test]
    public void stress5() 
    {
         CircularQueue<int?> queue = new CircularQueue<int?>( 3 );

        // we will setup two threads waiting to put to the queue,
        // then in a single synchronized step, read two items from
        // the queue. the first thread will be woken to put, and
        // once done the second thread should be woken by the first.

        queue.Put( 0 );
        queue.Put( 1 );
        queue.Put( 2 );
        Assert.IsTrue( queue.IsFull() );

        Thread t1 = new Thread( 
            delegate()
            {
                try
                {
                    Assert.IsTrue( queue.Put( 3 ) );
                }
                catch ( ThreadInterruptedException e )
                {
                    Console.WriteLine(e);
                }
            }
         );

        Thread t2 = new Thread(
            delegate()
            {
                try
                {
                    Assert.IsTrue( queue.Put( 4 ) );
                }
                catch ( ThreadInterruptedException e )
                {
                    Console.WriteLine(e);
                }
            }
        );

        t1.Start();
        t2.Start();

        // wait until both threads are waiting on queue...

        Thread.Sleep(100);

        lock (queue)
        {
            Assert.IsNotNull(queue.Get());
            Assert.IsNotNull(queue.Get());
        }

        harvest(t1);
        harvest(t2);
    }

    [Test]
    public void stress6()
    {
        CircularQueue<int?> queue = new CircularQueue<int?>(5);

        // start two getters and two putters and let 'em duke it out...

        Thread t1 = new Thread(
            delegate()
            {
                try
                {
                    for (int i = 0; i < 100; i++)
                        Assert.IsTrue(queue.Put(i));
                }
                catch (ThreadInterruptedException e)
                {
                    Console.WriteLine(e);
                }
            }
        );

        Thread t2 = new Thread(
            delegate()
            {
                try
                {
                    for (int i = 0; i < 100; i++)
                        Assert.IsTrue(queue.Put(i));
                }
                catch (ThreadInterruptedException e)
                {
                    Console.WriteLine(e);
                }
            }
        );

        Thread t3 = new Thread(
            delegate()
            {
                try
                {
                    for (int i = 0; i < 100; i++)
                        Assert.IsNotNull(queue.Get());
                }
                catch (ThreadInterruptedException e)
                {
                    Console.WriteLine(e);
                }
            }
        );

        Thread t4 = new Thread(
            delegate()
            {
                try
                {
                    for (int i = 0; i < 100; i++)
                        Assert.IsNotNull(queue.Get());
                }
                catch (ThreadInterruptedException e)
                {
                    Console.WriteLine(e);
                }
            }
        );

        t1.Start();
        t2.Start();
        t3.Start();
        t4.Start();

        harvest(t1);
        harvest(t2);
        harvest(t3);
        harvest(t4);
    }

    [Test]
    public void harvest1()
    {
        Thread t = new Thread(
            delegate()
            {
            }
        );
        t.Start();
        harvest(t);
    }

    [Test]
    [ExpectedException(typeof(TimeoutException))]
    public void harvest2()
    {
        Thread t = new Thread(
            delegate()
            {
                try
                {
                    Thread.Sleep(10000);
                }
                catch (ThreadInterruptedException e)
                {
                    Console.WriteLine(e);
                }
            }
        );
        t.Start();
        harvest(t);
    } 

    private void harvest(Thread t)
    {
        t.Join(1000);
        if (t.IsAlive)
        {
            t.Interrupt();
            throw new TimeoutException(t.Name + " is stuck");
        }
    }


    [Test]
    public void testRelError1()
    {
        assertRelError(10, 9, .11);
        assertRelError(10, 11, .14);
        assertRelError(20, 19, .07);
        assertRelError(19, 23, .22);
    }


    [Test]
    [ExpectedException(typeof(AssertionException))]
    public void testRelError2()
    {
        assertRelError(9, 8, .1);
    }

    [Test]
    [ExpectedException(typeof(AssertionException))]
    public void testRelError3()
    {
        assertRelError(9, 10, .1);
    }

    [Test]
    public void testAbsError1()
    {
        AssertAbsError(11, 15, 4);
        AssertAbsError(15, 10, 5);
        AssertAbsError(5, 3, 2);
        AssertAbsError(4, 7, 3);
    }

    [Test]
    [ExpectedException(typeof(AssertionException))]
    public void testAbsError2()
    {
        AssertAbsError(11, 15, 3);
    }

    [Test]
    [ExpectedException(typeof(AssertionException))]
    public void testAbsError3()
    {
        AssertAbsError(19, 15, 3);
    } 

    private void Delay(  int delay, run del )
    {

        Thread t = new Thread(delegate()
                                  {
                                      try
                                      {
                                          Thread.Sleep(delay);
                                          del();
                                      }
                                      catch (Exception e)
                                      {
                                          Console.WriteLine(e);
                                      }
                                  });
        t.Start();
    }

	private void assertRelError( double expected, double actual, double error )
	{
		double relError = RelError( expected, actual );
		if (relError > error)
			Assert.Fail( String.Format( "expected: {0} but was: {1} relative error: {2}", expected, actual, relError ) );
	}
	
	private void AssertAbsError( double expected, double actual, double error )
	{
		double absError = AbsError( expected, actual );
		if (absError > error)
           
			Assert.Fail( String.Format( "expected: {0} but was: {1} absolute error: {2}", expected, actual, absError ) );
	}
	
	private double RelError( double expected, double actual )
	{
		return AbsError( expected, actual ) / expected;
	}
	
	private double AbsError( double expected, double actual )
	{
		return Math.Abs( expected - actual );
	}

    }
}
