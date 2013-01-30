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
using NUnit.Framework;
using org.apache.etch.tests;
using org.apache.etch.tests.types.Test1;

namespace etch.tests
{
    [TestFixture]
    public class TestTest1DotCsharp
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestTest" );
        }


        [Test]
	    public void const_booleans()  
	    {
		    Assert.IsFalse( ConstsTest1.BOOL1 );
		    Assert.IsTrue( ConstsTest1.BOOL2 );
	    }
    	
	    [Test]
	    public void const_bytes()  
	    {
		    Assert.AreEqual( sbyte.MinValue, ConstsTest1.BYTE1 );
		    Assert.AreEqual( (sbyte) -1, ConstsTest1.BYTE2 );
		    Assert.AreEqual( (sbyte) 0, ConstsTest1.BYTE3 );
		    Assert.AreEqual( (sbyte) 1, ConstsTest1.BYTE4 );
		    Assert.AreEqual( sbyte.MaxValue, ConstsTest1.BYTE5 );
	    }
    	
	    [Test]
	    public void const_shorts()  
	    {
            
		    Assert.AreEqual( short.MinValue, ConstsTest1.SHORT1 );
		    Assert.AreEqual( (short) -1, ConstsTest1.SHORT2 );
		    Assert.AreEqual( (short) 0, ConstsTest1.SHORT3 );
		    Assert.AreEqual( (short) 1, ConstsTest1.SHORT4 );
		    Assert.AreEqual( short.MaxValue, ConstsTest1.SHORT5 );
	    }
    	
	    [Test]
	    public void const_ints()  
	    {
            Assert.AreEqual(int.MinValue, ConstsTest1.INT1);
            Assert.AreEqual(-1, ConstsTest1.INT2);
            Assert.AreEqual(0, ConstsTest1.INT3);
            Assert.AreEqual(1, ConstsTest1.INT4);
            Assert.AreEqual(int.MaxValue, ConstsTest1.INT5);
	    }
    	
	    [Test]
	    public void const_longs()  
	    {
            Assert.AreEqual(long.MinValue, ConstsTest1.LONG1);
            Assert.AreEqual((long)-1, ConstsTest1.LONG2);
            Assert.AreEqual((long)0, ConstsTest1.LONG3);
            Assert.AreEqual((long)1, ConstsTest1.LONG4);
            Assert.AreEqual(long.MaxValue, ConstsTest1.LONG5);
	    }
    	
	    [Test]
	    public void const_floats()  
	    {
            Assert.AreEqual(1.4e-45f, ConstsTest1.FLOAT1);
            Assert.AreEqual((float)-1, ConstsTest1.FLOAT2);
            Assert.AreEqual((float)0, ConstsTest1.FLOAT3);
            Assert.AreEqual((float)1, ConstsTest1.FLOAT4);
            Assert.AreEqual(3.4028235e38f, ConstsTest1.FLOAT5);
	    }
    	
	    [Test]
	    public void const_doubles()  
	    {
            Assert.AreEqual(4.9e-324, ConstsTest1.DOUBLE1);
            Assert.AreEqual((double)-1, ConstsTest1.DOUBLE2);
            Assert.AreEqual((double)0, ConstsTest1.DOUBLE3);
            Assert.AreEqual((double)1, ConstsTest1.DOUBLE4);
            Assert.AreEqual(1.7976931348623157e308, ConstsTest1.DOUBLE5);
	    }
    	
	    [Test]
	    public void const_strings()  
	    {
            Assert.AreEqual("", ConstsTest1.STRING1);
            Assert.AreEqual("a", ConstsTest1.STRING2);
            Assert.AreEqual("abc", ConstsTest1.STRING3);
            Assert.AreEqual("a\tb\tc\r\n", ConstsTest1.STRING4);
            Assert.AreEqual("x x", ConstsTest1.STRING5);
            Assert.AreEqual("x\u00bfx", ConstsTest1.STRING6);
            Assert.AreEqual("x\u03a9x", ConstsTest1.STRING7);
            Assert.AreEqual("x\\x", ConstsTest1.STRING8);
            Assert.AreEqual("x'x", ConstsTest1.STRING9);
            Assert.AreEqual("x\"x", ConstsTest1.STRING10);
	    }
    	
	    [Test]
	    public void enum_E1()  
	    {
            //Assert.AreEqual( 3, E1.values().Length );
		    Assert.IsNotNull( E1.A );
		    Assert.IsNotNull( E1.B );
		    Assert.IsNotNull( E1.C );
            //Assert.AreEqual( E1.A, E1.valueOf( "A" ) );
            //Assert.AreEqual( E1.B, E1.valueOf( "B" ) );
            //Assert.AreEqual( E1.C, E1.valueOf( "C" ) );
            //Assert.AreEqual( 0, E1.A.ordinal() );
            //Assert.AreEqual( 1, E1.B.ordinal() );
            //Assert.AreEqual( 2, E1.C.ordinal() );
	    }
    	
	    [Test]
	    public void struct_S1()  
	    {
		    S1 s = new S1( 1, 2, 3 );
		    Assert.AreEqual( 1, s.x );
		    Assert.AreEqual( 2, s.y );
		    Assert.AreEqual( 3, s.z );
    		
		    s = new S1( null, null, null );
		    Assert.IsNull( s.x );
		    Assert.IsNull( s.y );
		    Assert.IsNull( s.z );
    		
		    s = new S1();
		    Assert.IsNull( s.x );
		    Assert.IsNull( s.y );
		    Assert.IsNull( s.z );
    		
		    s.x = 4;
		    s.y = 5;
		    s.z = 6;
    		
		    Assert.AreEqual( 4, s.x );
		    Assert.AreEqual( 5, s.y );
		    Assert.AreEqual( 6, s.z );
    		
		    s.x = null;
		    Assert.IsNull( s.x );
    		
		    s.x = 7;
		    Assert.AreEqual( 7, s.x );
	    }
    	
	    [Test]
	    public void except_Excp1()  
	    {
		    Excp1 e = new Excp1( "foo", 23 );
		    Assert.AreEqual( "foo", e.msg );
		    Assert.AreEqual( 23, e.code );
    		
		    e = new Excp1( null, null );
		    Assert.IsNull( e.msg );
		    Assert.IsNull( e.code );
    		
		    e = new Excp1();
		    Assert.IsNull( e.msg );
		    Assert.IsNull( e.code );
    		
		    e.msg = "bar";
		    e.code = 24;
    		
		    Assert.AreEqual( "bar", e.msg );
		    Assert.AreEqual( 24, e.code );
    		
		    e.msg = null;
		    e.code = null;
    		
		    Assert.IsNull( e.msg );
		    Assert.IsNull( e.code );

            //Throwable t = e;
            //Assert.IsTrue( t is Exception );
            //Assert.IsFalse( t is RuntimeException );
	    }
    	
	    [Test]
	    public void except_Excp2()  
	    {
		    Excp2 e = new Excp2();

		    Exception t = e;
		    Assert.IsTrue( t is Exception );
		    //Assert.IsTrue( t is RuntimeException );
	    }
    	
	    [Test]
	    public void except_Excp3()  
	    {
		    Excp3 e = new Excp3();

            Exception t = e;
            Assert.IsTrue( t is Exception );
            //Assert.IsTrue( t is RuntimeException );
	    }
    	
	    [Test]
	    public void except_Excp4()  
	    {
		    Excp4 e = new Excp4();

		    Exception t = e;
		    Assert.IsTrue( t is Exception );
		    //Assert.IsFalse( t is Exception );
	    }
    	
	    [Test]
	    public void method_nothing()  
	    {
		    test.nothing();
	    }
    	
	    [Test]
	    public void method_incr1()  
	    {
		    Assert.AreEqual( 2, test.incr( 1 ) );
	    }
    	
	    [Test]
	    public void method_incr2()  
	    {
		    Assert.AreEqual( 3, test.incr( 2 ) );
	    }
    	
	    [Test]
	    public void method_incr3()  
	    {
		    Assert.AreEqual( -1, test.incr( -2 ) );
	    }
    	
	    [Test]
	    public void method_sub1()  
	    {
		    Assert.AreEqual( 5, test.sub( 7, 2 ) );
	    }
    	
	    [Test]
	    public void method_sub2()  
	    {
		    Assert.AreEqual( 8, test.sub( 23, 15 ) );
	    }
    	
	    [Test]
	    public void method_sub3()  
	    {
		    Assert.AreEqual( -5, test.sub( 2, 7 ) );
	    }
    	
	    [Test]
	    public void method_sum()  
	    {
		    Assert.AreEqual( 24, test.sum( new int[] { 1, 2, 3, 7, 11 } ) );
	    }
    	
	    [Test]
	    public void method_trans1()  
	    {
		    Assert.AreEqual( 2, test.trans( E1.A, 5 ) ); // divide by 2
	    }
    	
	    [Test]
	    public void method_trans2()  
	    {
		    Assert.AreEqual( 10, test.trans( E1.B, 5 ) ); // mul by 2
	    }
    	
	    [Test]
	    public void method_trans3()  
	    {
		    Assert.AreEqual( 12, test.trans( E1.C, 5 ) ); // add 7
	    }
    	
	    [Test]
	    public void method_dist1()  
	    {
		    Assert.AreEqual( Math.Sqrt( 3 ), test.dist( new S1( 1, 1, 1 ), new S1( 0, 0, 0 ) ) );
	    }
    	
	    [Test]
	    public void method_dist2()  
	    {
		    Assert.AreEqual( Math.Sqrt( 35 ), test.dist( new S1( 1, 2, 3 ), new S1( 6, 5, 4 ) ) );
	    }
    	
	    [Test]
	    public void method_dist3()  
	    {
		    Assert.AreEqual( Math.Sqrt( 56 ), test.dist( new S1( 1, 2, 3 ), new S1( -1, -2, -3 ) ) );
	    }
    	
	    [Test]
	    public void method_fill1()  
	    {
		    int[] x = test.fill( 0, 1 );
		    Assert.AreEqual( 0, x.Length );
		    foreach (int i in x)
			    Assert.AreEqual( 1, i );
	    }
    	
	    [Test]
	    public void method_fill2()  
	    {
		    int[] x = test.fill( 1, 2 );
		    Assert.AreEqual( 1, x.Length );
		    foreach (int i in x)
			    Assert.AreEqual( 2, i );
	    }
    	
	    [Test]
	    public void method_fill3()  
	    {
		    int[] x = test.fill( 2, 3 );
		    Assert.AreEqual( 2, x.Length );
		    foreach (int i in x)
			    Assert.AreEqual( 3, i );
	    }
    	
	    [Test]
	    public void method_blow1()  
	    {
		    try
		    {
			    test.blow( "foo", 2 );
                Assert.IsTrue( false, "blow did not throw" );
		    }
		    catch( Excp1 e )
		    {
			    Assert.AreEqual( "foo", e.msg );
			    Assert.AreEqual( 2, e.code );
		    }
	    }
    	
	    [Test]
	    public void method_blow2()  
	    {
		    try
		    {
			    test.blow( "bar", 3 );
                Assert.IsTrue( false, "blow did not throw" );
		    }
		    catch( Excp1 e )
		    {
			    Assert.AreEqual( "bar", e.msg );
			    Assert.AreEqual( 3, e.code );
		    }
	    }
    	
	    [Test]
	    public void method_beets1()  
	    {
		    Assert.AreEqual( 5, test.beets( E1.A ) );
	    }
    	
	    [Test]
	    public void method_beets2()  
	    {
		    try
		    {
			    test.beets( E1.B );
                Assert.IsTrue( false, "beets did not throw" );
		    }
		    catch ( Excp3 )
		    {
			    Assert.IsTrue( true );
		    }
	    }
    	
	    [Test]
	    public void method_beets3()  
	    {
		    try
		    {
			    test.beets( E1.C );
			    Assert.IsTrue(false, "beets did not throw" );
		    }
		    catch ( Excp4 )
		    {
			    Assert.IsTrue( true );
		    }
	    }
    	
        //[Test]
        //public void method_beets4()  
        //{
        //    Assert.IsNull( test.beets( null ) );
        //}

     
	 
	    //
        //Verify that an object can be passed as a parameter in a stuct.
        //
        [Test]
	    public void struct_S3() 
	    {
		    testS3Helper( "bool", ConstsTest1.BOOL2 );
            testS3Helper( "byte", ConstsTest1.BYTE5 );
            testS3Helper( "short", ConstsTest1.SHORT5 );
            testS3Helper("int", ConstsTest1.INT5);
            testS3Helper("long", ConstsTest1.LONG5);
            testS3Helper("float", ConstsTest1.FLOAT5);
            testS3Helper("double", ConstsTest1.DOUBLE5);
            testS3Helper("string", ConstsTest1.STRING3);		
	    }
    	
	    /*
	     * Verify that an array of objects can be passed as a parameter 
	     * in a stuct.
	     */
	    [Test]
	    public void struct_S4() 
	    {		
		    int sz = 10;
    		
		    testS4Helper(sz, "boolean", ConstsTest1.BOOL2 );
		    testS4Helper(sz, "byte", ConstsTest1.BYTE5 );
		    testS4Helper(sz, "short", ConstsTest1.SHORT5 );
            testS4Helper(sz, "int", ConstsTest1.INT5);
            testS4Helper(sz, "long", ConstsTest1.LONG5);
            testS4Helper(sz, "float", ConstsTest1.FLOAT5);
            testS4Helper(sz, "double", ConstsTest1.DOUBLE5);
            testS4Helper(sz, "string", ConstsTest1.STRING3);		

	    }
    	
	    // Verify that an object can be passed as a parameter in an
	    // exception
	    [Test]
	    public void except_Excp5() 
	    {
		    int code = 500;
		    String msg = "Excp5";

            testExcp5Helper(msg, code, ConstsTest1.BOOL1);
		    testExcp5Helper (msg, code, ConstsTest1.BYTE5);
		    testExcp5Helper (msg, code, ConstsTest1.SHORT5);
            testExcp5Helper(msg, code, ConstsTest1.INT5);
            testExcp5Helper(msg, code, ConstsTest1.LONG5);
            testExcp5Helper(msg, code, ConstsTest1.FLOAT5);
            testExcp5Helper(msg, code, ConstsTest1.DOUBLE5);
            testExcp5Helper(msg, code, ConstsTest1.STRING3);		
	    }
    	
	    [Test]
	    public void except_Excp6() 
	    {
		    int sz = 10;
		    int code = 500;
		    String msg = "Excp6";

            testExcp6Helper(sz, msg, code, ConstsTest1.BOOL1);
		    testExcp6Helper( sz, msg, code, ConstsTest1.BYTE5 );
		    testExcp6Helper( sz, msg, code, ConstsTest1.SHORT5 );
            testExcp6Helper(sz, msg, code, ConstsTest1.INT5);
            testExcp6Helper(sz, msg, code, ConstsTest1.LONG5);
            testExcp6Helper(sz, msg, code, ConstsTest1.FLOAT5);
            testExcp6Helper(sz, msg, code, ConstsTest1.DOUBLE5);
            testExcp6Helper(sz, msg, code, ConstsTest1.STRING3);		
	    }
    	
	    [Test]
	    public void method_throwExcp5() 
	    {
		    String msg = "throwExcp5";
		    int code = 500;

            method_throwExcp5Helper(msg, code, ConstsTest1.BOOL1);
		    method_throwExcp5Helper( msg, code, ConstsTest1.BYTE5 );
		    method_throwExcp5Helper( msg, code, ConstsTest1.SHORT5 );
            method_throwExcp5Helper(msg, code, ConstsTest1.INT5);
            method_throwExcp5Helper(msg, code, ConstsTest1.LONG5);
            method_throwExcp5Helper(msg, code, ConstsTest1.FLOAT5);
            method_throwExcp5Helper(msg, code, ConstsTest1.DOUBLE5);
            method_throwExcp5Helper(msg, code, ConstsTest1.STRING3);		
	    }
    	
	    [Test]
	    public void method_throwExcp6() 
	    {
		    String msg = "throwExcp6";
		    int code = 500;
		    int sz = 10;

            method_throwExcp6Helper(sz, msg, code, ConstsTest1.BOOL1);
		    method_throwExcp6Helper( sz, msg, code, ConstsTest1.BYTE5 );
		    method_throwExcp6Helper( sz, msg, code, ConstsTest1.SHORT5 );
            method_throwExcp6Helper(sz, msg, code, ConstsTest1.INT5);
            method_throwExcp6Helper(sz, msg, code, ConstsTest1.LONG5);
            method_throwExcp6Helper(sz, msg, code, ConstsTest1.FLOAT5);
            method_throwExcp6Helper(sz, msg, code, ConstsTest1.DOUBLE5);
            method_throwExcp6Helper(sz, msg, code, ConstsTest1.STRING3);		

	    }

        #region Helper methods

        private void testS3Helper( string type, object value )
	    {
		    S3 s = new S3( type, value );
		    Assert.AreEqual( type, s.tipe );
		    Assert.AreEqual( value, s.x );
    		
		    s = new S3( null, null );
            Assert.IsNull(s.tipe);
		    Assert.IsNull( s.x );
    		
		    s = new S3();
		    Assert.IsNull( s.tipe );
		    Assert.IsNull( s.x );
    		
		    s.tipe = type;
		    s.x = value;	
		    Assert.AreEqual( type, s.tipe );
		    Assert.AreEqual( value, s.x );
    		
		    s.x = null;
		    Assert.IsNull( s.x );
    		
		    s.x = value;
		    Assert.AreEqual( value, s.x );
	    }

        public void testS4Helper( int sz, string type, object value )
        {
            object[] myObject = test.fillObject( sz, value );
            S4 s = new S4( type, myObject );
            Assert.AreEqual( type, s.tipe );
            Assert.AreEqual( sz, myObject.Length );

            for ( int i = 0; i < myObject.Length; i++ )
                Assert.AreEqual( value, myObject[ i ] );
        }


        private void testExcp5Helper( String msg, int code, Object value )
	    {
		    Excp5 e = new Excp5( msg, code, value );
		    Assert.AreEqual( msg, e.msg );
		    Assert.AreEqual( code, e.code );
		    Assert.AreEqual( value, e.x);
    		
		    e = new Excp5( null, null, null );
		    Assert.IsNull( e.msg );
		    Assert.IsNull( e.code );
		    Assert.IsNull( e.x);

		    e = new Excp5();
		    Assert.IsNull( e.msg );
		    Assert.IsNull( e.code );
		    Assert.IsNull( e.x );
    		
		    e.msg = msg;
		    e.code = code;
		    e.x = value;
    		
		    Assert.AreEqual( msg, e.msg );
		    Assert.AreEqual( code, e.code );
		    Assert.AreEqual( value, e.x );
    		
		    e.msg = null;
		    e.code = null;
		    e.x = null;
    		
		    Assert.IsNull( e.msg );
		    Assert.IsNull( e.code );
		    Assert.IsNull( e.x );		

		    //Throwable t = e;
		    Assert.IsTrue( e is Exception );
		    //assertFalse( t instanceof RuntimeException );
	    }

        private void testExcp6Helper( int sz, String msg, int code, Object value )
        {
	        Object[] myObject = test.fillObject( sz, value );
    		
	        Excp6 e = new Excp6( msg, code, myObject );
	        Assert.AreEqual( msg, e.msg );
	        Assert.AreEqual( code, e.code );

	        for ( int i = 0; i < myObject.Length; i++ ) 
	        {
		        Assert.AreEqual( value, myObject[i] );
	        }
    		
	        e = new Excp6( null, null, null );
	        Assert.IsNull( e.msg );
	        Assert.IsNull( e.code );
	        Assert.IsNull( e.x );

	        e = new Excp6();
	        Assert.IsNull( e.msg );
	        Assert.IsNull( e.code );
	        Assert.IsNull( e.x );
    		
	        e.msg = msg;
	        e.code = code;
	        e.x = myObject;
    		
	        Assert.AreEqual( msg, e.msg );
	        Assert.AreEqual( code, e.code );
    		
	        for ( int i = 0; i < myObject.Length ; i++ ) 
	        {
		        Assert.AreEqual( value, myObject[i] );
	        }
    		
	        e.msg = null;
	        e.code = null;
	        e.x = null;
    		
	        Assert.IsNull( e.msg );
	        Assert.IsNull( e.code );
	        Assert.IsNull( e.x );		


	        //Exception t = e;
	        Assert.IsTrue( e is Exception );
	        // assertFalse( t is RuntimeException );
        }

        private void method_throwExcp5Helper( String msg, int code, Object value )
        {
            try
            {
                test.throwExcp5( msg, code, value );
                Assert.Fail( "throwExcp5 did not throw" );
            }
            catch ( Excp5 e )
            {
                Assert.AreEqual( msg, e.msg );
                Assert.AreEqual( code, e.code );
                Assert.AreEqual( value, e.x );
            }
        }

        private void method_throwExcp6Helper( int sz, String msg, int code, Object value )
        {
            Object[] myObject = test.fillObject( sz, value );

            try
            {
                test.throwExcp6( msg, code, myObject );
                Assert.Fail( "throwExcp5 did not throw" );
            }
            catch ( Excp6 e )
            {
                Assert.AreEqual( msg, e.msg );
                Assert.AreEqual( code, e.code );

                for ( int i = 0; i < myObject.Length; i++ )
                {
                    Assert.AreEqual( value, e.x[ i ] );
                }
            }
        }

        private void method_fillObjectHelper( int sz, Object value )
        {
            Object[] myObject = test.fillObject( sz, value );

            for ( int i = 0; i < myObject.Length; i++ )
            {
                Assert.AreEqual( value, myObject[ i ] );
            }
        }

        #endregion

        private Test1 test = new FakeTest1();
    }
}
