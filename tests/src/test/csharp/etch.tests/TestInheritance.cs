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
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;
using org.apache.etch.tests;
using org.apache.etch.tests.types.Inheritance;

namespace etch.tests
{
    [TestFixture]
    public class TestInheritance
    {
        private static RemoteInheritanceServer server;
        private static ServerFactory listener;
        
        [TestFixtureSetUp]
        public void Setup()
        {
            String uri = "tcp://localhost:4003";

            MainInheritanceListener implFactory = new MainInheritanceListener();
            listener = InheritanceHelper.NewListener(uri, null, implFactory);

            listener.TransportControl(TransportConsts.START_AND_WAIT_UP, 4000);

            MainInheritanceClient client = new MainInheritanceClient();
            server = InheritanceHelper.NewServer(uri, null, client);

        //    server._TransportControl(Etch.Transport.TransportConsts.START_AND_WAIT_UP, 4000);

            server._StartAndWaitUp(4000);
        }

        [Test]
	    public void types() 
	    {
		    Assert.AreEqual(true,typeof(S1).IsAssignableFrom(typeof(S1)));
            Assert.AreEqual(true,typeof(S1).IsAssignableFrom(typeof(S2)));
		    Assert.AreEqual(true,typeof(S1).IsAssignableFrom(typeof(S3)));

            Assert.AreEqual(false,typeof(S2).IsAssignableFrom(typeof(S1)));
            Assert.AreEqual(true,typeof(S2).IsAssignableFrom(typeof(S2)));
		    Assert.AreEqual(true,typeof(S2).IsAssignableFrom(typeof(S3)));
		
            Assert.AreEqual(false,typeof(S3).IsAssignableFrom(typeof(S1)));
            Assert.AreEqual(false,typeof(S3).IsAssignableFrom(typeof(S2)));
		    Assert.AreEqual(true,typeof(S3).IsAssignableFrom(typeof(S3)));
		   
    		Assert.AreEqual(true,typeof(E1).IsAssignableFrom(typeof(E1)));
            Assert.AreEqual(true,typeof(E1).IsAssignableFrom(typeof(E2)));
		    Assert.AreEqual(true,typeof(E1).IsAssignableFrom(typeof(E3)));
		    
    		Assert.AreEqual(true,typeof(Exception).IsAssignableFrom(typeof(E1)));
            Assert.AreEqual(true,typeof(Exception).IsAssignableFrom(typeof(E2)));
            Assert.AreEqual(true,typeof(Exception).IsAssignableFrom(typeof(E3)));
		 
    		Assert.AreEqual(false,typeof(E2).IsAssignableFrom(typeof(E1)));
            Assert.AreEqual(true,typeof(E2).IsAssignableFrom(typeof(E2)));
		    Assert.AreEqual(true,typeof(E2).IsAssignableFrom(typeof(E3)));
		    
    		Assert.AreEqual(false,typeof(E3).IsAssignableFrom(typeof(E1)));
            Assert.AreEqual(false,typeof(E3).IsAssignableFrom(typeof(E2)));
		    Assert.AreEqual(true,typeof(E3).IsAssignableFrom(typeof(E3)));
		   
	    }
	
	
	    [Test]
	    public void Construct_S1() 
	    {
		    check_s1( new S1(), null, null );
		
		    check_s1( new S1( null, null ), null, null );
		
		    check_s1( new S1( 1, 2 ), 1, 2 );
	    }
	
	
	    [Test]
	    public void construct_s2() 
	    {
		    check_s2( new S2(), null, null, null, null );
    		
		    check_s2( new S2( null, null, null, null ), null, null, null, null );
    		
		    check_s2( new S2( 1, 2, 3, 4 ), 1, 2, 3, 4 );
	    }
	
	
	    [Test]
	    public void construct_s3() 
	    {
		    check_s3( new S3(), null, null, null, null, null, null );
    		
		    check_s3( new S3( null, null, null, null, null, null ),
			    null, null, null, null, null, null );
    		
		    check_s3( new S3( 1, 2, 3, 4, 5, 6 ), 1, 2, 3, 4, 5, 6 );
	    }
	
	
	    [Test]
	    public void construct_e1() 
	    {
		    check_e1( new E1(), null, null );
    		
		    check_e1( new E1( null, null ), null, null );
    		
		    check_e1( new E1( 1, 2 ), 1, 2 );
	    }
	
	
	    [Test]
	    public void construct_e2() 
	    {
		    check_e2( new E2(), null, null, null, null );
    		
		    check_e2( new E2( null, null, null, null ), null, null, null, null );
    		
		    check_e2( new E2( 1, 2, 3, 4 ), 1, 2, 3, 4 );
	    }
	
	
	    [Test]
	    public void construct_e3() 
	    {
		    check_e3( new E3(), null, null, null, null, null, null );
    		
		    check_e3( new E3( null, null, null, null, null, null ),
			    null, null, null, null, null, null );
    		
		    check_e3( new E3( 1, 2, 3, 4, 5, 6 ), 1, 2, 3, 4, 5, 6 );
	    }
	
	
	    [Test]
	    public void assign_s1() 
	    {
		    S1 v = new S1( 1, 2 ); 
            
            check_s1( v, 1, 2 );
    		
		    v.a = null;            check_s1( v, null, 2 );
		    v.a = 3;               check_s1( v, 3, 2 );
		    v.SetA( null );        check_s1( v, null, 2 );
		    v.SetA( 4 );           check_s1( v, 4, 2 );
    		
		    v.b = null;            check_s1( v, 4, null );
		    v.b = 5;               check_s1( v, 4, 5 );
		    v.SetB( null );        check_s1( v, 4, null );
		    v.SetB( 6 );           check_s1( v, 4, 6 );
	    }
	
	
	    [Test]
	    public void assign_s2()
	    {
		    S2 v = new S2( 1, 2, 3, 4 ); 
            
            check_s2( v, 1, 2, 3, 4 );
    		
		    v.a = null;                  check_s2( v, null, 2, 3, 4 );
		    v.a = 5;                     check_s2( v, 5, 2, 3, 4 );
		    v.SetA( null );              check_s2( v, null, 2, 3, 4 );
		    v.SetA( 6 );                 check_s2( v, 6, 2, 3, 4 );
    		
		    v.b = null;                  check_s2( v, 6, null, 3, 4 );
		    v.b = 7;                     check_s2( v, 6, 7, 3, 4 );
		    v.SetB( null );              check_s2( v, 6, null, 3, 4 );
		    v.SetB( 8 );                 check_s2( v, 6, 8, 3, 4 );
    		
		    v.c = null;                  check_s2( v, 6, 8, null, 4 );
		    v.c = 9;                     check_s2( v, 6, 8, 9, 4 );
		    v.SetC( null );              check_s2( v, 6, 8, null, 4 );
		    v.SetC( 10 );                check_s2( v, 6, 8, 10, 4 );
    		
		    v.d = null;                  check_s2( v, 6, 8, 10, null );
		    v.d = 11;                    check_s2( v, 6, 8, 10, 11 );
		    v.SetD( null );              check_s2( v, 6, 8, 10, null );
		    v.SetD( 12 );                check_s2( v, 6, 8, 10, 12 );
	    }
	
	
	    [Test]
	    public void assign_s3() 
	    {
		    S3 v = new S3( 1, 2, 3, 4, 5, 6 ); 
            check_s3( v, 1, 2, 3, 4, 5, 6 );
    		
		    v.a = null;                        check_s3( v, null, 2, 3, 4, 5, 6 );
		    v.a = 7;                           check_s3( v, 7, 2, 3, 4, 5, 6 );
		    v.SetA( null );                    check_s3( v, null, 2, 3, 4, 5, 6 );
		    v.SetA( 8 );                       check_s3( v, 8, 2, 3, 4, 5, 6 );
    		
		    v.b = null;                        check_s3( v, 8, null, 3, 4, 5, 6 );
		    v.b = 9;                           check_s3( v, 8, 9, 3, 4, 5, 6 );
		    v.SetB( null );                    check_s3( v, 8, null, 3, 4, 5, 6 );
		    v.SetB( 10 );                      check_s3( v, 8, 10, 3, 4, 5, 6 );
    		
		    v.c = null;                        check_s3( v, 8, 10, null, 4, 5, 6 );
		    v.c = 11;                          check_s3( v, 8, 10, 11, 4, 5, 6 );
		    v.SetC( null );                    check_s3( v, 8, 10, null, 4, 5, 6 );
		    v.SetC( 12 );                      check_s3( v, 8, 10, 12, 4, 5, 6 );
    		
		    v.d = null;                        check_s3( v, 8, 10, 12, null, 5, 6 );
		    v.d = 13;                          check_s3( v, 8, 10, 12, 13, 5, 6 );
		    v.SetD( null );                    check_s3( v, 8, 10, 12, null, 5, 6 );
		    v.SetD( 14 );                      check_s3( v, 8, 10, 12, 14, 5, 6 );
    		
		    v.e = null;                        check_s3( v, 8, 10, 12, 14, null, 6 );
		    v.e = 15;                          check_s3( v, 8, 10, 12, 14, 15, 6 );
		    v.SetE( null );                    check_s3( v, 8, 10, 12, 14, null, 6 );
		    v.SetE( 16 );                      check_s3( v, 8, 10, 12, 14, 16, 6 );
    		
		    v.f = null;                        check_s3( v, 8, 10, 12, 14, 16, null );
		    v.f = 17;                          check_s3( v, 8, 10, 12, 14, 16, 17 );
		    v.SetF( null );                    check_s3( v, 8, 10, 12, 14, 16, null );
		    v.SetF( 18 );                      check_s3( v, 8, 10, 12, 14, 16, 18 );
	    }
	
	
	   [Test]
	    public void tostring() 
	    {
		    Assert.AreEqual(
			    "S1(a=1; b=2)",
			    new S1( 1, 2 ).ToString() );
    		
		    Assert.AreEqual(
			    "S2(S1(a=1; b=2); c=3; d=4)",
			    new S2( 1, 2, 3, 4 ).ToString() );
    		
		    Assert.AreEqual(
			    "S3(S2(S1(a=1; b=2); c=3; d=4); e=5; f=6)",
			    new S3( 1, 2, 3, 4, 5, 6 ).ToString() );
    		
		    Assert.AreEqual(
                "a=1; b=2",
			    new E1( 1, 2 ).GetMessage() );
    		
		    Assert.AreEqual(
                "a=1; b=2; c=3; d=4",
			    new E2( 1, 2, 3, 4 ).GetMessage() );
    		
		    Assert.AreEqual(
                "a=1; b=2; c=3; d=4; e=5; f=6",
			    new E3( 1, 2, 3, 4, 5, 6 ).GetMessage() );
	    }

	    private void check_s1( S1 v, int? a, int? b )
	    {
    		
            Assert.AreEqual( a, v.a );
		    Assert.AreEqual( a, v.GetA() );
    		
		    Assert.AreEqual( b, v.b );
		    Assert.AreEqual( b, v.GetB() );
	    }

	    private void check_s2( S2 v, int? a, int? b, int? c, int? d )
	    {
		    check_s1( v, a, b );
    		
		    Assert.AreEqual( c, v.c );
		    Assert.AreEqual( c, v.GetC() );
    		
		    Assert.AreEqual( d, v.d );
		    Assert.AreEqual( d, v.GetD() );
	    }

	    private void check_s3( S3 v, int? a, int? b, int? c, int? d,
		    int? e, int? f )
	    {
		    check_s2( v, a, b, c, d );
    		
		    Assert.AreEqual( e, v.e );
		    Assert.AreEqual( e, v.GetE() );
    		
		    Assert.AreEqual( f, v.f );
		    Assert.AreEqual( f, v.GetF() );
	    }

	    private void check_e1( E1 v, int? a, int? b )
	    {
		    Assert.AreEqual( a, v.a );
		    Assert.AreEqual( a, v.GetA() );
    		
		    Assert.AreEqual( b, v.b );
		    Assert.AreEqual( b, v.GetB() );
	    }

	    private void check_e2( E2 v, int? a, int? b, int? c, int? d )
	    {
		    check_e1( v, a, b );
    		
		    Assert.AreEqual( c, v.c );
		    Assert.AreEqual( c, v.GetC() );
    		
		    Assert.AreEqual( d, v.d );
		    Assert.AreEqual( d, v.GetD() );
	    }

	    private void check_e3( E3 v, int? a, int? b, int? c, int? d,
		    int? e, int? f )
	    {
		    check_e2( v, a, b, c, d );
    		
		    Assert.AreEqual( e, v.e );
		    Assert.AreEqual( e, v.GetE() );
    		
		    Assert.AreEqual( f, v.f );
		    Assert.AreEqual( f, v.GetF() );
	    }

	
	    [Test]
	    public void test_s1() 
	    {
		    do_s1( new S1( 1, 2 ), new S1Compare() );
		    do_s1( new S1( 9, 8 ), new S1Compare() );
	    }

	
	    [Test]
	    public void test_s2() 
	    {
		    do_s2( new S2( 1, 2, 3, 4 ), new S2Compare() );
		    do_s2( new S2( 9, 8, 7, 6 ), new S2Compare() );
	    }


	    [Test]
	    public void test_s3() 
	    {
		    do_s3( new S3( 1, 2, 3, 4, 5, 6 ), new S3Compare() );
		    do_s3( new S3( 9, 8, 7, 6, 5, 4 ), new S3Compare() );
	    } 
	
	    interface CompareObj<T>
	    {
    		
		    void compare( T a, T b );
	    }
	
	
	
	/**
	 * Class to compare one S2 to another.
	 */
	 class S2Compare : CompareObj<object>
	{
		public void compare( object a, object b )
		{
            Assert.AreEqual(typeof(S2), a.GetType());
			Assert.AreEqual( typeof(S2), b.GetType() );
            S2 a1 = (S2)a;
            S2 b1 = (S2)b;
			Assert.AreEqual( a1.a, b1.a );
			Assert.AreEqual( a1.b, b1.b );
			Assert.AreEqual( a1.c, b1.c );
			Assert.AreEqual( a1.d, b1.d );
		}
	} 

	/**
	 * Class to compare one S3 to another.
	 */
	class S3Compare : CompareObj<object>
	{
		public void compare( object a, object b )
		{
			Assert.AreEqual( typeof(S3), a.GetType() );
			Assert.AreEqual( typeof(S3), b.GetType() );
            S3 a1 = (S3)a;
            S3 b1 = (S3)b;
			Assert.AreEqual( a1.a, b1.a );
			Assert.AreEqual( a1.b, b1.b );
			Assert.AreEqual( a1.c, b1.c );
			Assert.AreEqual( a1.d, b1.d );
			Assert.AreEqual( a1.e, b1.e );
			Assert.AreEqual( a1.f, b1.f );
		}
	} 
	
	 
	
	 class AS2Compare : CompareObj<object[]>
	{
		
		public AS2Compare( CompareObj<object> cmp )
		{
			this.cmp = cmp;
		}
		
		private CompareObj<object> cmp;

		
		public void compare( object[] a, object[] b )
		{
            S2[] a1 = (S2[])a;
            S2[] b1 = (S2[])b;
			Assert.AreEqual( a1.Length, b1.Length );
			for (int i = 0; i < a.Length; i++)
				cmp.compare( a1[i], b1[i] );
		}
	}
	
	 class AS3Compare : CompareObj<object[]>
	{
		
		public AS3Compare( CompareObj<object> cmp )
		{
			this.cmp = cmp;
		}
		
		private  CompareObj<object> cmp;

		
		public void compare( object[] a, object[] b )
		{
			Assert.AreEqual( a.Length, b.Length );
			for (int i = 0; i < a.Length; i++)
				cmp.compare( a[i], b[i] );
		}
	} 

	/////////////
	// HELPERS //
	/////////////


        private void do_obj(object v, CompareObj<object> cmp)
	    {
		    // test an object both by itself and as an array.
    		
		    cmp.compare( v,server.f1( v ) );

            object[] a = { v };
	        CompareObj<object[]> acmp = new AObjectCompare( cmp );
    		
		    acmp.compare( a, server.f5( a ) );  
	    }

  
	private void do_s1( S1 v, CompareObj<object> cmp )
	{
		// test an S1 both by itself and as an array.
		// also test each of those as an object.
		
		cmp.compare( v, server.f2( v ) );
		do_obj( v, cmp );
		
		S1[] a = { v };
        CompareObj<object[]> acmp = new AS1Compare(cmp);

		acmp.compare( a, server.f6( a ) );
//		do_obj( a, acmp );  
	}

    /**
 * Class to compare one S1 to another.
 */
    class S1Compare : CompareObj<object>
    {
        public void compare(object a, object b)
        {
            Assert.AreEqual(typeof(S1), a.GetType());
            Assert.AreEqual(typeof(S1), b.GetType());
            S1 a1 = (S1)a;
            S1 b1 = (S1)b;
            Assert.AreEqual(a1.a, b1.a);
            Assert.AreEqual(a1.b, b1.b);
        }
    }

    class AObjectCompare : CompareObj<Object[]>
    {

        public AObjectCompare(CompareObj<Object> cmp)
        {
            this.cmp = cmp;
        }

        private CompareObj<Object> cmp;


        public void compare(Object[] a, Object[] b)
        {
            Assert.AreEqual(a.Length, b.Length);
            for (int i = 0; i < a.Length; i++)
                cmp.compare(a[i], b[i]);
        }
    }

    class AS1Compare : CompareObj<object[]>
    {

        public AS1Compare(CompareObj<object> cmp)
        {
            this.cmp = cmp;
        }

        private CompareObj<object> cmp;


        public void compare(object[] a, object[] b)
        {
            S1[] a1 = (S1[])a;
            S1[] b1 = (S1[])b;
            Assert.AreEqual(a1.Length, b1.Length);
            for (int i = 0; i < a1.Length; i++)
                cmp.compare(a1[i], b1[i]);
        }
    }


	
	 private void do_s2( S2 v, CompareObj<object> cmp )
	{
		// test an S2 both by itself and as an array.
		// also test the S2 as an S1, and the S2 array
		// as an object.
		
		cmp.compare( v, server.f3( v ) );
		do_s1( v, cmp );
		
		S2[] a = { v };
		CompareObj<object[]> acmp = new AS2Compare( cmp );

		acmp.compare( a, server.f7( a ) );
	//	do_obj( a, acmp );
	}

	
	private void do_s3( S3 v, CompareObj<object> cmp )
	{
		// test an S3 both by itself and as an array.
		// also test the S3 as an S2, and the S3 array
		// as an object.
		
		cmp.compare( v, server.f4( v ) );
		do_s2( v, cmp );
		
		S3[] a = { v };
		CompareObj<object[]> acmp = new AS3Compare( cmp );

		acmp.compare( a, server.f8( a ) );
	//	do_obj( a, acmp );
	} 

	
	  [TestFixtureTearDown]
	public void Dispose() 
	{
		if (server != null)
			server._StopAndWaitDown( 4000 );
        if (listener != null)
            listener.TransportControl(TransportConsts.STOP_AND_WAIT_DOWN, 4000);
	}
}



    }
