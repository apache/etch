using System;
using System.Collections.Generic;
using System.Text;

using NUnit.Framework;
using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using etch.bindings.csharp.compiler.test;

namespace Etch.Compiler.Test
{
    [TestFixture]
    public class TestTestDotCsharp
    {

        [Test]
	    public void const_booleans()  
	    {
		    Assert.IsFalse( etch.bindings.csharp.compiler.test.ConstsTest.BOOL1 );
		    Assert.IsTrue( etch.bindings.csharp.compiler.test.ConstsTest.BOOL2 );
	    }
    	
	    [Test]
	    public void const_bytes()  
	    {
		    Assert.AreEqual( sbyte.MinValue, etch.bindings.csharp.compiler.test.ConstsTest.BYTE1 );
		    Assert.AreEqual( (sbyte) -1, etch.bindings.csharp.compiler.test.ConstsTest.BYTE2 );
		    Assert.AreEqual( (sbyte) 0, etch.bindings.csharp.compiler.test.ConstsTest.BYTE3 );
		    Assert.AreEqual( (sbyte) 1, etch.bindings.csharp.compiler.test.ConstsTest.BYTE4 );
		    Assert.AreEqual( sbyte.MaxValue, etch.bindings.csharp.compiler.test.ConstsTest.BYTE5 );
	    }
    	
	    [Test]
	    public void const_shorts()  
	    {
		    Assert.AreEqual( short.MinValue, etch.bindings.csharp.compiler.test.ConstsTest.SHORT1 );
		    Assert.AreEqual( (short) -1, etch.bindings.csharp.compiler.test.ConstsTest.SHORT2 );
		    Assert.AreEqual( (short) 0, etch.bindings.csharp.compiler.test.ConstsTest.SHORT3 );
		    Assert.AreEqual( (short) 1, etch.bindings.csharp.compiler.test.ConstsTest.SHORT4 );
		    Assert.AreEqual( short.MaxValue, etch.bindings.csharp.compiler.test.ConstsTest.SHORT5 );
	    }
    	
	    [Test]
	    public void const_ints()  
	    {
		    Assert.AreEqual( int.MinValue, etch.bindings.csharp.compiler.test.ConstsTest.INT1 );
		    Assert.AreEqual( -1, etch.bindings.csharp.compiler.test.ConstsTest.INT2 );
		    Assert.AreEqual( 0, etch.bindings.csharp.compiler.test.ConstsTest.INT3 );
		    Assert.AreEqual( 1, etch.bindings.csharp.compiler.test.ConstsTest.INT4 );
		    Assert.AreEqual( int.MaxValue, etch.bindings.csharp.compiler.test.ConstsTest.INT5 );
	    }
    	
	    [Test]
	    public void const_longs()  
	    {
		    Assert.AreEqual( long.MinValue, etch.bindings.csharp.compiler.test.ConstsTest.LONG1 );
		    Assert.AreEqual( (long) -1, etch.bindings.csharp.compiler.test.ConstsTest.LONG2 );
		    Assert.AreEqual( (long) 0, etch.bindings.csharp.compiler.test.ConstsTest.LONG3 );
		    Assert.AreEqual( (long) 1, etch.bindings.csharp.compiler.test.ConstsTest.LONG4 );
		    Assert.AreEqual( long.MaxValue, etch.bindings.csharp.compiler.test.ConstsTest.LONG5 );
	    }
    	
	    [Test]
	    public void const_floats()  
	    {
            Assert.AreEqual( 1.4e-45f, etch.bindings.csharp.compiler.test.ConstsTest.FLOAT1 );
		    Assert.AreEqual( (float) -1, etch.bindings.csharp.compiler.test.ConstsTest.FLOAT2 );
		    Assert.AreEqual( (float) 0, etch.bindings.csharp.compiler.test.ConstsTest.FLOAT3 );
		    Assert.AreEqual( (float) 1, etch.bindings.csharp.compiler.test.ConstsTest.FLOAT4 );
            Assert.AreEqual( 3.4028235e38f, etch.bindings.csharp.compiler.test.ConstsTest.FLOAT5 );
	    }
    	
	    [Test]
	    public void const_doubles()  
	    {
            Assert.AreEqual( 4.9e-324, etch.bindings.csharp.compiler.test.ConstsTest.DOUBLE1 );
		    Assert.AreEqual( (double) -1, etch.bindings.csharp.compiler.test.ConstsTest.DOUBLE2 );
		    Assert.AreEqual( (double) 0, etch.bindings.csharp.compiler.test.ConstsTest.DOUBLE3 );
		    Assert.AreEqual( (double) 1, etch.bindings.csharp.compiler.test.ConstsTest.DOUBLE4 );
            Assert.AreEqual( 1.7976931348623157e308, etch.bindings.csharp.compiler.test.ConstsTest.DOUBLE5 );
	    }
    	
	    [Test]
	    public void const_strings()  
	    {
		    Assert.AreEqual( "", etch.bindings.csharp.compiler.test.ConstsTest.STRING1 );
		    Assert.AreEqual( "a", etch.bindings.csharp.compiler.test.ConstsTest.STRING2 );
		    Assert.AreEqual( "abc", etch.bindings.csharp.compiler.test.ConstsTest.STRING3 );
		    Assert.AreEqual( "a\tb\tc\r\n", etch.bindings.csharp.compiler.test.ConstsTest.STRING4 );
		    Assert.AreEqual( "x x", etch.bindings.csharp.compiler.test.ConstsTest.STRING5 );
		    Assert.AreEqual( "x\u00bfx", etch.bindings.csharp.compiler.test.ConstsTest.STRING6 );
		    Assert.AreEqual( "x\u03a9x", etch.bindings.csharp.compiler.test.ConstsTest.STRING7 );
		    Assert.AreEqual( "x\\x", etch.bindings.csharp.compiler.test.ConstsTest.STRING8 );
		    Assert.AreEqual( "x'x", etch.bindings.csharp.compiler.test.ConstsTest.STRING9 );
		    Assert.AreEqual( "x\"x", etch.bindings.csharp.compiler.test.ConstsTest.STRING10 );
	    }
    	
	    [Test]
	    public void enum_E1()  
	    {
            //Assert.AreEqual( 3, etch.bindings.csharp.compiler.test.ConstsTest.E1.values().Length );
		    Assert.IsNotNull( etch.bindings.csharp.compiler.test.ConstsTest.E1.A );
		    Assert.IsNotNull( etch.bindings.csharp.compiler.test.ConstsTest.E1.B );
		    Assert.IsNotNull( etch.bindings.csharp.compiler.test.ConstsTest.E1.C );
            //Assert.AreEqual( etch.bindings.csharp.compiler.test.ConstsTest.E1.A, etch.bindings.csharp.compiler.test.ConstsTest.E1.valueOf( "A" ) );
            //Assert.AreEqual( etch.bindings.csharp.compiler.test.ConstsTest.E1.B, etch.bindings.csharp.compiler.test.ConstsTest.E1.valueOf( "B" ) );
            //Assert.AreEqual( etch.bindings.csharp.compiler.test.ConstsTest.E1.C, etch.bindings.csharp.compiler.test.ConstsTest.E1.valueOf( "C" ) );
            //Assert.AreEqual( 0, etch.bindings.csharp.compiler.test.ConstsTest.E1.A.ordinal() );
            //Assert.AreEqual( 1, etch.bindings.csharp.compiler.test.ConstsTest.E1.B.ordinal() );
            //Assert.AreEqual( 2, etch.bindings.csharp.compiler.test.ConstsTest.E1.C.ordinal() );
	    }
    	
	    [Test]
	    public void struct_S1()  
	    {
		    etch.bindings.csharp.compiler.test.ConstsTest.S1 s = new etch.bindings.csharp.compiler.test.ConstsTest.S1( 1, 2, 3 );
		    Assert.AreEqual( 1, s.x );
		    Assert.AreEqual( 2, s.y );
		    Assert.AreEqual( 3, s.z );
    		
		    s = new etch.bindings.csharp.compiler.test.ConstsTest.S1( null, null, null );
		    Assert.IsNull( s.x );
		    Assert.IsNull( s.y );
		    Assert.IsNull( s.z );
    		
		    s = new etch.bindings.csharp.compiler.test.ConstsTest.S1();
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
		    etch.bindings.csharp.compiler.test.ConstsTest.Excp1 e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp1( "foo", 23 );
		    Assert.AreEqual( "foo", e.msg );
		    Assert.AreEqual( 23, e.code );
    		
		    e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp1( null, null );
		    Assert.IsNull( e.msg );
		    Assert.IsNull( e.code );
    		
		    e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp1();
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
		    etch.bindings.csharp.compiler.test.ConstsTest.Excp2 e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp2();

		    Exception t = e;
		    Assert.IsTrue( t is Exception );
		    //Assert.IsTrue( t is RuntimeException );
	    }
    	
	    [Test]
	    public void except_Excp3()  
	    {
		    etch.bindings.csharp.compiler.test.ConstsTest.Excp3 e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp3();

            Exception t = e;
            Assert.IsTrue( t is Exception );
            //Assert.IsTrue( t is RuntimeException );
	    }
    	
	    [Test]
	    public void except_Excp4()  
	    {
		    etch.bindings.csharp.compiler.test.ConstsTest.Excp4 e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp4();

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
		    Assert.AreEqual( 2, test.trans( etch.bindings.csharp.compiler.test.ConstsTest.E1.A, 5 ) ); // divide by 2
	    }
    	
	    [Test]
	    public void method_trans2()  
	    {
		    Assert.AreEqual( 10, test.trans( etch.bindings.csharp.compiler.test.ConstsTest.E1.B, 5 ) ); // mul by 2
	    }
    	
	    [Test]
	    public void method_trans3()  
	    {
		    Assert.AreEqual( 12, test.trans( etch.bindings.csharp.compiler.test.ConstsTest.E1.C, 5 ) ); // add 7
	    }
    	
	    [Test]
	    public void method_dist1()  
	    {
		    Assert.AreEqual( Math.Sqrt( 3 ), test.dist( new etch.bindings.csharp.compiler.test.ConstsTest.S1( 1, 1, 1 ), new etch.bindings.csharp.compiler.test.ConstsTest.S1( 0, 0, 0 ) ) );
	    }
    	
	    [Test]
	    public void method_dist2()  
	    {
		    Assert.AreEqual( Math.Sqrt( 35 ), test.dist( new etch.bindings.csharp.compiler.test.ConstsTest.S1( 1, 2, 3 ), new etch.bindings.csharp.compiler.test.ConstsTest.S1( 6, 5, 4 ) ) );
	    }
    	
	    [Test]
	    public void method_dist3()  
	    {
		    Assert.AreEqual( Math.Sqrt( 56 ), test.dist( new etch.bindings.csharp.compiler.test.ConstsTest.S1( 1, 2, 3 ), new etch.bindings.csharp.compiler.test.ConstsTest.S1( -1, -2, -3 ) ) );
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
		    catch( etch.bindings.csharp.compiler.test.ConstsTest.Excp1 e )
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
		    catch( etch.bindings.csharp.compiler.test.ConstsTest.Excp1 e )
		    {
			    Assert.AreEqual( "bar", e.msg );
			    Assert.AreEqual( 3, e.code );
		    }
	    }
    	
	    [Test]
	    public void method_beets1()  
	    {
		    Assert.AreEqual( 5, test.beets( etch.bindings.csharp.compiler.test.ConstsTest.E1.A ) );
	    }
    	
	    [Test]
	    public void method_beets2()  
	    {
		    try
		    {
			    test.beets( etch.bindings.csharp.compiler.test.ConstsTest.E1.B );
                Assert.IsTrue( false, "beets did not throw" );
		    }
		    catch ( etch.bindings.csharp.compiler.test.ConstsTest.Excp3 )
		    {
			    Assert.IsTrue( true );
		    }
	    }
    	
	    [Test]
	    public void method_beets3()  
	    {
		    try
		    {
			    test.beets( etch.bindings.csharp.compiler.test.ConstsTest.E1.C );
			    Assert.IsTrue(false, "beets did not throw" );
		    }
		    catch ( etch.bindings.csharp.compiler.test.ConstsTest.Excp4 )
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
		    testS3Helper( "bool", ConstsTest.BOOL2 );
            testS3Helper( "byte", ConstsTest.BYTE5 );
            testS3Helper( "short", ConstsTest.SHORT5 );
            testS3Helper( "int", ConstsTest.INT5 );
            testS3Helper( "long", ConstsTest.LONG5 );
            testS3Helper( "float", ConstsTest.FLOAT5 );
            testS3Helper( "double", ConstsTest.DOUBLE5 );
            testS3Helper( "string", ConstsTest.STRING3 );		
	    }
    	
	    /*
	     * Verify that an array of objects can be passed as a parameter 
	     * in a stuct.
	     */
	    [Test]
	    public void struct_S4() 
	    {		
		    int sz = 10;
    		
		    testS4Helper(sz, "boolean", ConstsTest.BOOL2 );
		    testS4Helper(sz, "byte", ConstsTest.BYTE5 );
		    testS4Helper(sz, "short", ConstsTest.SHORT5 );
		    testS4Helper(sz, "int", ConstsTest.INT5 );
		    testS4Helper(sz, "long", ConstsTest.LONG5 );
		    testS4Helper(sz, "float", ConstsTest.FLOAT5 );
		    testS4Helper(sz, "double", ConstsTest.DOUBLE5 );
		    testS4Helper(sz, "string", ConstsTest.STRING3 );		

	    }
    	
	    // Verify that an object can be passed as a parameter in an
	    // exception
	    [Test]
	    public void except_Excp5() 
	    {
		    int code = 500;
		    String msg = "Excp5";
    		
		    testExcp5Helper (msg, code, ConstsTest.BOOL1);
		    testExcp5Helper (msg, code, ConstsTest.BYTE5);
		    testExcp5Helper (msg, code, ConstsTest.SHORT5);
		    testExcp5Helper (msg, code, ConstsTest.INT5);
		    testExcp5Helper (msg, code, ConstsTest.LONG5);
		    testExcp5Helper (msg, code, ConstsTest.FLOAT5);
		    testExcp5Helper (msg, code, ConstsTest.DOUBLE5);
		    testExcp5Helper (msg, code, ConstsTest.STRING3);		
	    }
    	
	    [Test]
	    public void except_Excp6() 
	    {
		    int sz = 10;
		    int code = 500;
		    String msg = "Excp6";
    		
		    testExcp6Helper( sz, msg, code, ConstsTest.BOOL1 );
		    testExcp6Helper( sz, msg, code, ConstsTest.BYTE5 );
		    testExcp6Helper( sz, msg, code, ConstsTest.SHORT5 );
		    testExcp6Helper( sz, msg, code, ConstsTest.INT5 );
		    testExcp6Helper( sz, msg, code, ConstsTest.LONG5 );
		    testExcp6Helper( sz, msg, code, ConstsTest.FLOAT5 );
		    testExcp6Helper( sz, msg, code, ConstsTest.DOUBLE5 );
		    testExcp6Helper( sz, msg, code, ConstsTest.STRING3 );		
	    }
    	
	    [Test]
	    public void method_throwExcp5() 
	    {
		    String msg = "throwExcp5";
		    int code = 500;

		    method_throwExcp5Helper( msg, code, ConstsTest.BOOL1 );
		    method_throwExcp5Helper( msg, code, ConstsTest.BYTE5 );
		    method_throwExcp5Helper( msg, code, ConstsTest.SHORT5 );
		    method_throwExcp5Helper( msg, code, ConstsTest.INT5 );
		    method_throwExcp5Helper( msg, code, ConstsTest.LONG5 );
		    method_throwExcp5Helper( msg, code, ConstsTest.FLOAT5 );
		    method_throwExcp5Helper( msg, code, ConstsTest.DOUBLE5 );
		    method_throwExcp5Helper( msg, code, ConstsTest.STRING3 );		
	    }
    	
	    [Test]
	    public void method_throwExcp6() 
	    {
		    String msg = "throwExcp6";
		    int code = 500;
		    int sz = 10;

		    method_throwExcp6Helper( sz, msg, code, ConstsTest.BOOL1 );
		    method_throwExcp6Helper( sz, msg, code, ConstsTest.BYTE5 );
		    method_throwExcp6Helper( sz, msg, code, ConstsTest.SHORT5 );
		    method_throwExcp6Helper( sz, msg, code, ConstsTest.INT5 );
		    method_throwExcp6Helper( sz, msg, code, ConstsTest.LONG5 );
		    method_throwExcp6Helper( sz, msg, code, ConstsTest.FLOAT5 );
		    method_throwExcp6Helper( sz, msg, code, ConstsTest.DOUBLE5 );
		    method_throwExcp6Helper( sz, msg, code, ConstsTest.STRING3 );		

	    }

        #region Helper methods

        private void testS3Helper( string type, object value )
	    {
		    etch.bindings.csharp.compiler.test.ConstsTest.S3 s = new etch.bindings.csharp.compiler.test.ConstsTest.S3( type, value );
		    Assert.AreEqual( type, s.type );
		    Assert.AreEqual( value, s.x );
    		
		    s = new etch.bindings.csharp.compiler.test.ConstsTest.S3( null, null );
		    Assert.IsNull( s.type );
		    Assert.IsNull( s.x );
    		
		    s = new etch.bindings.csharp.compiler.test.ConstsTest.S3();
		    Assert.IsNull( s.type );
		    Assert.IsNull( s.x );
    		
		    s.type = type;
		    s.x = value;	
		    Assert.AreEqual( type, s.type );
		    Assert.AreEqual( value, s.x );
    		
		    s.x = null;
		    Assert.IsNull( s.x );
    		
		    s.x = value;
		    Assert.AreEqual( value, s.x );
	    }

        public void testS4Helper( int sz, string type, object value )
        {
            object[] myObject = test.fillObject( sz, value );
            etch.bindings.csharp.compiler.test.ConstsTest.S4 s = new etch.bindings.csharp.compiler.test.ConstsTest.S4( type, myObject );
            Assert.AreEqual( type, s.type );
            Assert.AreEqual( sz, myObject.Length );

            for ( int i = 0; i < myObject.Length; i++ )
                Assert.AreEqual( value, myObject[ i ] );
        }


        private void testExcp5Helper( String msg, int code, Object value )
	    {
		    etch.bindings.csharp.compiler.test.ConstsTest.Excp5 e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp5( msg, code, value );
		    Assert.AreEqual( msg, e.msg );
		    Assert.AreEqual( code, e.code );
		    Assert.AreEqual( value, e.x);
    		
		    e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp5( null, null, null );
		    Assert.IsNull( e.msg );
		    Assert.IsNull( e.code );
		    Assert.IsNull( e.x);

		    e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp5();
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
    		
	        etch.bindings.csharp.compiler.test.ConstsTest.Excp6 e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp6( msg, code, myObject );
	        Assert.AreEqual( msg, e.msg );
	        Assert.AreEqual( code, e.code );

	        for ( int i = 0; i < myObject.Length; i++ ) 
	        {
		        Assert.AreEqual( value, myObject[i] );
	        }
    		
	        e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp6( null, null, null );
	        Assert.IsNull( e.msg );
	        Assert.IsNull( e.code );
	        Assert.IsNull( e.x );

	        e = new etch.bindings.csharp.compiler.test.ConstsTest.Excp6();
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
            catch ( etch.bindings.csharp.compiler.test.ConstsTest.Excp5 e )
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
            catch ( etch.bindings.csharp.compiler.test.ConstsTest.Excp6 e )
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

        private etch.bindings.csharp.compiler.test.Test test = new FakeTest();
    }
}
