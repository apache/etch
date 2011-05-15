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
using System.Globalization;
using Etch4.Util.Test;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    [TestFixture]
    public class TestMonitor
    {

    private const double REL_ERROR = 100000;
    private CultureInfo savedCulture;

    [SetUp]
    public void SetUp()
    {
        savedCulture = Thread.CurrentThread.CurrentCulture;
        Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;
    }

    [TearDown]
    public void TearDown()
    {
        Thread.CurrentThread.CurrentCulture = savedCulture;
    }

	/** @ */
	[Test]
	public void construct1() 
	{
		testconstruct1( null );
		testconstruct1( "a" );
		testconstruct1( "b" );
	}
	
	private void testconstruct1( String descr )
	{
		Monitor<Object> m = new Monitor<Object>( descr );
		Assert.AreEqual( descr, m.GetDescription() );
		Assert.IsNull( m.Get() );
	}

	/** @ */
	[Test]
	public void construct2() 
	{
		testconstruct2( null, null );
		testconstruct2( null, true );
		testconstruct2( null, 1 );
		testconstruct2( null, "abc" );
		testconstruct2( null, 3.14159 );
		
		testconstruct2( "a", null );
		testconstruct2( "b", true );
		testconstruct2( "c", 1 );
		testconstruct2( "d", "abc" );
		testconstruct2( "e", 3.14159 );
	}
	
	private void testconstruct2( String descr, Object value )
	{
		Monitor<Object> m = new Monitor<Object>( descr, value );
		Assert.AreEqual( descr, m.GetDescription() );
		Assert.AreEqual( value, m.Get() );
	}
	
	/** @ */
	[Test]
	public void mToString() 
	{
        Assert.AreEqual("Monitor : ", new Monitor<Object>(null, null).ToString());
		Assert.AreEqual( "Monitor : True", new Monitor<Object>( null, true ).ToString() );
		Assert.AreEqual( "Monitor : 1", new Monitor<Object>( null, 1 ).ToString() );
		Assert.AreEqual( "Monitor : abc", new Monitor<Object>( null, "abc" ).ToString() );
		Assert.AreEqual( "Monitor : 3.14159", new Monitor<Object>( null, 3.14159 ).ToString() );

		Assert.AreEqual( "Monitor a: ", new Monitor<Object>( "a", null ).ToString() );
		Assert.AreEqual( "Monitor b: True", new Monitor<Object>( "b", true ).ToString() );
		Assert.AreEqual( "Monitor c: 1", new Monitor<Object>( "c", 1 ).ToString() );
		Assert.AreEqual( "Monitor d: abc", new Monitor<Object>( "d", "abc" ).ToString() );
		Assert.AreEqual( "Monitor e: 3.14159", new Monitor<Object>( "e", 3.14159 ).ToString() );
	}
	
	/** @ */
	[Test]
	public void Set1() 
	{
		Monitor<Object> m = new Monitor<Object>( "a", null );
		Assert.AreEqual( null, m.Set( 1 ) );
		Assert.AreEqual( 1, m.Set( 2 ) );
		Assert.AreEqual( 2, m.Set( null ) );
		Assert.AreEqual( null, m.Set( null ) );
	}
	
	/** @ */
	[Test]
	public void Set2() 
	{
		Monitor<Object> m = new Monitor<Object>( "a", 1 );
		Assert.AreEqual( 1, m.Set( 2 ) );
		Assert.AreEqual( 2, m.Set( null ) );
		Assert.AreEqual( null, m.Set( null ) );
	}
	
	/** @ */
	[Test]
	public void SetGet() 
	{
		Monitor<Object> m = new Monitor<Object>( "a", 1 );
		
		Assert.AreEqual( 1, m.Set( 2 ) );
		Assert.AreEqual( 2, m.Get() );
		
		Assert.AreEqual( 2, m.Set( null ) );
		Assert.AreEqual( null, m.Get() );
		
		Assert.AreEqual( null, m.Set( null ) );
		Assert.AreEqual( null, m.Get() );
	}
	
	/** @ */
        [Ignore]
	[Test]
	public void Setsync() 
	{
		 Monitor<int?> m = new Monitor<int?>( "a", null );
		 bool[] flag = new bool[2000];

        Thread t = new Thread(new ThreadStart( delegate()
                                                   {
                                                       for (int i = 0; i < 1000; i++)
				{
					int? x = m.Set( i );
					
						flag[x.GetValueOrDefault()] = true;
				}
                                                   }));

	    Thread t1 = TestHelp.Delay(1000, t);


	    Thread t2 = TestHelp.Delay(1000, new Thread(new ThreadStart(delegate()
	                     {
                          for (int i = 1000; i < 2000; i++)
				         {
					            int? x = m.Set( i );					
						        flag[x.GetValueOrDefault()] = true;
				         }
	                                                                     
	                     }
                         )));
		

		t1.Start();
		t2.Start();
		
		t1.Join();
		t2.Join();
		
		int? x1 = m.Set( null );
		
			flag[x1.GetValueOrDefault()] = true;
		
		foreach (bool b  in flag)
			Assert.IsTrue( b );
	}
	
	/** @ */
	[Test]
	public void waitUntilEq1() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", null );
		m.WaitUntilEq( null );
		m.WaitUntilEq( null, 0 );
		
		m.Set( 1 );
		m.WaitUntilEq( 1 );
		m.WaitUntilEq( 1, 0 );
		
		m.Set( 2 );
		m.WaitUntilEq( 2 );
		m.WaitUntilEq( 2, 0 );
	}
	
	/** @ */
	[Test]
        [ExpectedException(typeof(ThreadInterruptedException))]
	public void waitUntilEq2() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", 1 );
		m.WaitUntilEq( 2, 1 );
	}
	
	/** @ */
        [Test]
        [ExpectedException(typeof(ThreadInterruptedException))]
	public void waitUntilEq3() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", null );
		m.WaitUntilEq( 2, 1 );
	}
	
	/** @ */
        [Test]
        [ExpectedException(typeof(ThreadInterruptedException))]
	public void waitUntilEq4() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", 1 );
		m.WaitUntilEq( null, 1 );
	}
	
	/** @ */
	[Test]
	public void waitUntilNotEq1() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", null );
		m.WaitUntilNotEq( 1 );
		m.WaitUntilNotEq( 1, 0 );
		
		m.Set( 1 );
		m.WaitUntilNotEq( null );
		m.WaitUntilNotEq( null, 0 );
	}
	
	/** @ */
        [Test]
        [ExpectedException(typeof(ThreadInterruptedException))]
	public void waitUntilNotEq2() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", null );
		m.WaitUntilNotEq( null, 1 );
	}
	
	/** @ */
        [Test]
        [ExpectedException(typeof(ThreadInterruptedException))]
	public void waitUntilNotEq3() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", 1 );
		m.WaitUntilNotEq( 1, 1 );
	}
	
	/** @ */
	[Test]
	public void waitUntilEqAndSet1() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", null );
		Assert.AreEqual( null, m.WaitUntilEqAndSet( null, 1 ) );
		Assert.AreEqual( 1, m.WaitUntilEqAndSet( 1, 2 ) );
		Assert.AreEqual( 2, m.WaitUntilEqAndSet( 2, null ) );
		Assert.AreEqual( null, m.WaitUntilEqAndSet( null, null ) );

		Assert.AreEqual( null, m.WaitUntilEqAndSet( null, 0, 1 ) );
		Assert.AreEqual( 1, m.WaitUntilEqAndSet( 1, 0, 2 ) );
		Assert.AreEqual( 2, m.WaitUntilEqAndSet( 2, 0, null ) );
		Assert.AreEqual( null, m.WaitUntilEqAndSet( null, 0, null ) );
	}
	
	/** @ */
        [Test]
        [ExpectedException(typeof(ThreadInterruptedException))]
	public void waitUntilEqAndSet2() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", null );
		try
		{
			m.WaitUntilEqAndSet( 1, 1, 2 );
		}
		finally
		{
			Assert.AreEqual( null, m.Get() );
		}
	}
	
	/** @ */
        [Test]
        [ExpectedException(typeof(ThreadInterruptedException))]
	public void waitUntilEqAndSet3() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", 1 );
		try
		{
			m.WaitUntilEqAndSet( null, 1, 2 );
		}
		finally
		{
			Assert.AreEqual( 1, m.Get() );
		}
	}
	
	/** @ */
	[Test]
	public void waitUntilNotEqAndSet1() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", null );
		Assert.AreEqual( null, m.WaitUntilNotEqAndSet( 2, 1 ) );
		Assert.AreEqual( 1, m.WaitUntilNotEqAndSet( 3, 2 ) );
		Assert.AreEqual( 2, m.WaitUntilNotEqAndSet( 4, null ) );
		Assert.AreEqual( null, m.WaitUntilNotEqAndSet( 5, null ) );

		Assert.AreEqual( null, m.WaitUntilNotEqAndSet( 2, 0, 1 ) );
		Assert.AreEqual( 1, m.WaitUntilNotEqAndSet( 3, 0, 2 ) );
		Assert.AreEqual( 2, m.WaitUntilNotEqAndSet( 4, 0, null ) );
		Assert.AreEqual( null, m.WaitUntilNotEqAndSet( 5, 0, null ) );
	}
	
	/** @ */
        [Test]
        [ExpectedException(typeof(ThreadInterruptedException))]
	public void waitUntilNotEqAndSet2() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", null );
		try
		{
			m.WaitUntilNotEqAndSet( null, 1, 1 );
		}
		finally
		{
			Assert.AreEqual( null, m.Get() );
		}
	}
	
	/** @ */
        [Test]
        [ExpectedException(typeof(ThreadInterruptedException))]
	public void waitUntilNotEqAndSet3() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", 1 );
		try
		{
			m.WaitUntilNotEqAndSet( 1, 1, null );
		}
		finally
		{
			Assert.AreEqual( 1, m.Get() );
		}
	}
	
	/** @ */
	[Test]
	public void waitUntilEq5() 
	{
	    Monitor<int?> m = new Monitor<int?>( "a", null );
		
		Thread t = TestHelp.delayAndStart( 200, new Thread(new ThreadStart(delegate()
		                                                                       {
		                                                                          m.Set( 1 ); 
		                                                                       })));


	    long t0 = HPTimer.Now();
		m.WaitUntilEq( 1 );
		double t1 = (HPTimer.Now() - t0) / 1000000.0;
		TestHelp.AssertRelError( 200, t1, REL_ERROR );
		
		t.Join();
	}
	
	/** @ */
	[Test]
	public void waitUntilNotEq4() 
	{
		   Monitor<int?> m = new Monitor<int?>( "a", null );
		
		Thread t = TestHelp.delayAndStart( 200, new Thread(new ThreadStart(delegate()
		                                                                       {
		                                                                          m.Set( 1 ); 
		                                                                       })));


		long t0 = HPTimer.Now();
		m.WaitUntilNotEq( null );
		double t1 = (HPTimer.Now() - t0) / 1000000.0;
		TestHelp.AssertRelError( 200, t1, REL_ERROR );
		
		t.Join();
	}
	
	/** @ */
	[Test]
	public void waitUntilEqAndSet4() 
	{
		Monitor<int?> m = new Monitor<int?>( "a", null );
		
		Thread t = TestHelp.delayAndStart( 200, new Thread(new ThreadStart(delegate()
		                                                                       {
		                                                                          m.Set( 1 ); 
		                                                                       })));

	    long t0 = HPTimer.Now();
		m.WaitUntilEqAndSet( 1, 2 );
		double t1 = (HPTimer.Now() - t0) / 1000000.0;
		// TODO is there a better way for choosing relative error?
		TestHelp.AssertRelError( 200, t1, REL_ERROR );
		
		t.Join();
	}
	
	/** @ */
	[Test]
	public void waitUntilNotEqAndSet4() 
	{
        Monitor<int?> m = new Monitor<int?>("a", null);

        Thread t = TestHelp.delayAndStart(200, new Thread(new ThreadStart(delegate()
                                                                               {
                                                                                   m.Set(1);
                                                                               })));

	    long t0 = HPTimer.Now();
		m.WaitUntilNotEqAndSet( null, 2 );
        double t1 = (HPTimer.Now() - t0) / 1000000.0;
		TestHelp.AssertRelError( 200, t1, REL_ERROR );
		
		t.Join();
	}

    }
}
