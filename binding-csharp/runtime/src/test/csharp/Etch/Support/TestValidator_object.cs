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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
        [TestFixture]
        public class TestValidator_object
        {
            private readonly ValueFactory vf = new DummyValueFactory();

            [TestFixtureSetUp]
            public void First()
            {
                Console.WriteLine();
                Console.Write( "TestValidator_object" );
            }

           
	    [Test]
	    public void constructor1()  
	    {
		    testconstructor( 0, "object[0]", typeof(Object) );
		    testconstructor( 1, "object[1]", typeof(Object[]) );
		    testconstructor( 2, "object[2]", typeof(Object[][]) );
		    testconstructor( 3, "object[3]", typeof(Object[][][]) );
		    testconstructor( 4, "object[4]", typeof(Object[][][][]));
		    testconstructor( 5, "object[5]", typeof(Object[][][][][]) );
		    testconstructor( 6, "object[6]", typeof(Object[][][][][][]) );
		    testconstructor( 7, "object[7]", typeof(Object[][][][][][][]) );
		    testconstructor( 8, "object[8]", typeof(Object[][][][][][][][]) );
		    testconstructor( 9, "object[9]", typeof(Object[][][][][][][][][]) );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
    	
	    private void testconstructor( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = Validator_object.Get( n );
		    Assert.AreEqual( n, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }
    	
    	
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor2()  
	    {
		    Validator_object.Get( -1 );
	    }
    	
    	
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor3()  
	    {
		    Validator_object.Get( Validator.MAX_NDIMS+1 );
	    }
    	
    	
	    [Test]
	    public void elementvalidator1()  
	    {
		    testelementvalidator( 0, 0, "object[0]", typeof(Object) );
		    testelementvalidator( 1, 0, "object[0]", typeof(Object) );
		    testelementvalidator( 2, 1, "object[1]", typeof(Object[]) );
		    testelementvalidator( 3, 2, "object[2]", typeof(Object[][]) );
		    testelementvalidator( 4, 3, "object[3]", typeof(Object[][][]) );
		    testelementvalidator( 5, 4, "object[4]", typeof(Object[][][][]) );
		    testelementvalidator( 6, 5, "object[5]", typeof(Object[][][][][]) );
		    testelementvalidator( 7, 6, "object[6]", typeof(Object[][][][][][]) );
		    testelementvalidator( 8, 7, "object[7]", typeof(Object[][][][][][][]) );
		    testelementvalidator( 9, 8, "object[8]", typeof(Object[][][][][][][][]) );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
    	
	    private void testelementvalidator( int n, int k, String descr, Type expectedClass )
	    {
		    TypeValidator v = (TypeValidator) Validator_object.Get( n ).ElementValidator();
		    Assert.AreEqual( k, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }
    	
	    /** @  */
	    [Test]
	    public void good_scalar()  
	    {
		    testgoodvalue( 0,  false );
		    testgoodvalue( 0,  true );
		    testgoodvalue( 0,  (sbyte) 1 );
		    testgoodvalue( 0,  (short) 2222 );
		    testgoodvalue( 0,  33333333 );
		    testgoodvalue( 0,  4444444444444444L );
		    testgoodvalue( 0,  5.5f );
		    testgoodvalue( 0,  6.6 );
		    testgoodvalue( 0,  "" );
		    testgoodvalue( 0,  "abc" );
		    testgoodvalue( 0,  new Object() );
            testgoodvalue(0, new StructValue(new XType("abc"), vf));
		    testgoodvalue( 0,  new DateTime() );
	    }
    	
	    /** @  */
	    [Test]
	    public void good_array()  
	    {
		    // primitive array types are compatible with object.
    		
		    testgoodvalue( 0,  new bool[] {} );
		    testgoodvalue( 0,  new sbyte[] {} );
		    testgoodvalue( 0,  new short[] {} );
		    testgoodvalue( 0,  new int[] {} );
		    testgoodvalue( 0,  new long[] {} );
		    testgoodvalue( 0,  new float[] {} );
		    testgoodvalue( 0,  new double[] {} );
    		
		    // ref array types are compatible with object.

		    testgoodvalue( 0,  new Boolean[] {} );
		    testgoodvalue( 0,  new SByte[] {} );
		    testgoodvalue( 0,  new Int16[] {} );
		    testgoodvalue( 0,  new Int32[] {} );
		    testgoodvalue( 0,  new Int64[] {} );
		    testgoodvalue( 0,  new Single[] {} );
		    testgoodvalue( 0,  new Double[] {} );
		    testgoodvalue( 0,  new String[] {} );
		    testgoodvalue( 0,  new Object[] {} );
		    testgoodvalue( 0,  new StructValue[] {} );
		    testgoodvalue( 0,  new DateTime[] {} );
    		
		    // ref array types are compatible with object array (same size).
	       
		/*    testgoodvalue( 1,  new Boolean[] {} );
		    testgoodvalue( 1,  new SByte[] {} );
		    testgoodvalue( 1,  new Int16[] {} ); 
		    testgoodvalue( 1,  new Int32[] {} );
		    testgoodvalue( 1,  new Int64[] {} );
		    testgoodvalue( 1,  new Single[] {} );
		    testgoodvalue( 1,  new Double[] {} ); */
		    testgoodvalue( 1,  new String[] {} ); 
		    testgoodvalue( 1,  new Object[] {} );
		    testgoodvalue( 1,  new StructValue[] {} );
//		    testgoodvalue( 1,  new DateTime[] {} );
    		
		    // ref array types are compatible with object array (larger size).
    		
/*		    testgoodvalue( 1,  new Boolean[][] {} );
		    testgoodvalue( 1,  new SByte[][] {} );
		    testgoodvalue( 1,  new Int16[][] {} );
		    testgoodvalue( 1,  new Int32[][] {} );
		    testgoodvalue( 1,  new Int64[][] {} );
		    testgoodvalue( 1,  new Single[][] {} );
		    testgoodvalue( 1,  new Double[][] {} ); */
		    testgoodvalue( 1,  new String[][] {} );
		    testgoodvalue( 1,  new Object[][] {} );
		    testgoodvalue( 1,  new StructValue[][] {} );
//		    testgoodvalue( 1,  new DateTime[][] {} );
	    }
    	
	    private void testgoodvalue( int n, Object value )
	    {
		    TypeValidator v = Validator_object.Get( n );
		    Assert.IsTrue( v.Validate( value ) );
		    Assert.IsTrue( validateValueOk( v, value ) );
	    }
    	
	    /** @  */
	    [Test]
	    public void assignable1()  
	    {
		    Object x;
    		
		    x = new Object();
		    Assert.IsTrue( x1( typeof(Object), typeof(Object) ) );

		    x = new Object[] {};
		    Assert.IsTrue( x1( typeof(Object), typeof(Object[]) ) );
    		
		    x = new Object[][] {};
		    Assert.IsTrue( x1( typeof(Object), typeof(Object[][]) ) );

		    x = new Int32();
		    Assert.IsTrue( x1( typeof(Object), typeof(Int32) ) );
    		
		    x = new Int32[] {};
		    Assert.IsTrue( x1( typeof(Object), typeof(Int32[]) ) );

		    x = new int[] {};
		    Assert.IsTrue( x1(typeof(Object), typeof(int) ) );
    		
		    Assert.IsNotNull( x );
	    }
    	
	    /** @  */
	    [Test]
	    public void assignable2()  
	    {
		    Object[] y;
    		
    //		y = new Object();
		    Assert.IsFalse( x1( typeof(Object[]), typeof(Object) ) );

		    y = new Object[] {};
		    Assert.IsTrue( x1(  typeof(Object[]),  typeof(Object[]) ) );
    		
		    y = new Object[][] {};
		    Assert.IsTrue( x1(  typeof(Object[]), typeof(Object[][]) ) );

    //		y = new Integer( 0 );
		    Assert.IsFalse( x1(  typeof(Object[]), typeof(Int32) ) );
    		
    		

    //		y = new int[] {};
		    Assert.IsFalse( x1(  typeof(Object[]), typeof(int[]) ) );
	    }
    	
    	
    	
	    private bool x1( Type a, Type b )
	    {
		    return a.IsAssignableFrom( b );
	    }

    	
	    [Test]
	    public void bad_scalar()  
	    {
		    testbadvalue( 0, null );
    //		testbadvalue( 0, false );
    //		testbadvalue( 0, true );
    //		testbadvalue( 0, (byte) 1 );
    //		testbadvalue( 0, (short) 2222 );
    //		testbadvalue( 0, 33333333 );
    //		testbadvalue( 0, 4444444444444444L );
    //		testbadvalue( 0, 5.5f );
    //		testbadvalue( 0, 6.6 );
    //		testbadvalue( 0, "" );
    //		testbadvalue( 0, "abc" );
    //		testbadvalue( 0, new Object() );
    //		testbadvalue( 0, new StructValue( new Type( "abc" ) ) );
    //		testbadvalue( 0, new Date() );

		    testbadvalue( 1, null );
		    testbadvalue( 1, false );
		    testbadvalue( 1, true );
		    testbadvalue( 1, (byte) 1 );
		    testbadvalue( 1, (short) 2222 );
		    testbadvalue( 1, 333333 );
		    testbadvalue( 1, 4444444444444444L );
		    testbadvalue( 1, 5.5f );
		    testbadvalue( 1, 6.6 );
		    testbadvalue( 1, "" );
		    testbadvalue( 1, "abc" );
		    testbadvalue( 1, new Object() );
            testbadvalue(1, new StructValue(new XType("abc"), vf));
		    testbadvalue( 1, new DateTime() );
	    }
    	
	    /** @  */
	    [Test]
	    public void bad_array()  
	    {
		    // primitive arrays cannot map into object arrays...
    		
		    testbadvalue( 1, new bool[] {} );
		    testbadvalue( 1, new sbyte[] {} );
		    testbadvalue( 1, new short[] {} );
		    testbadvalue( 1, new int[] {} );
		    testbadvalue( 1, new long[] {} );
		    testbadvalue( 1, new float[] {} );
		    testbadvalue( 1, new double[] {} );

		    // too short...
    		
		    testbadvalue( 2, new Object[] {} );
		    testbadvalue( 3, new Object[][] {} );
		    testbadvalue( 4, new Object[][][] {} );
		    testbadvalue( 5, new Object[][][][] {} );
		    testbadvalue( 6, new Object[][][][][] {} );
		    testbadvalue( 7, new Object[][][][][][] {} );
		    testbadvalue( 8, new Object[][][][][][][] {} );
		    testbadvalue( 9, new Object[][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }

	    private void testbadvalue( int n, Object value )
	    {
		    TypeValidator v = Validator_object.Get( n );
		    Assert.IsFalse( v.Validate( value ) );
		    Assert.IsFalse( validateValueOk( v, value ) );
	    }

	    private bool validateValueOk( Validator v, Object value )
	    {
		    try
		    {
			    Object x = v.ValidateValue( value );
			    Assert.AreEqual( value, x );
			    return true;
		    }
		    catch ( Exception)
		    {
               
			    return false;
		    }
	    }

       

    }
}
