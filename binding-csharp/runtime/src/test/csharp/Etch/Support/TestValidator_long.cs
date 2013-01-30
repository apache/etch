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
        public class TestValidator_long
        {
            private readonly ValueFactory vf = new DummyValueFactory();

            [TestFixtureSetUp]
            public void First()
            {
                Console.WriteLine();
                Console.Write( "TestValidator_long" );
            }

        [Test]
	    public void constructor1()  
	    {
		    testconstructor( 0, "long[0]", typeof(long) );
		    testconstructor( 1, "long[1]", typeof(long[]) );
		    testconstructor( 2, "long[2]", typeof(long[][]));
		    testconstructor( 3, "long[3]", typeof(long[][][]) );
		    testconstructor( 4, "long[4]", typeof(long[][][][]) );
		    testconstructor( 5, "long[5]", typeof(long[][][][][]) );
		    testconstructor( 6, "long[6]", typeof(long[][][][][][]) );
		    testconstructor( 7, "long[7]", typeof(long[][][][][][][]) );
		    testconstructor( 8, "long[8]", typeof(long[][][][][][][][]) );
		    testconstructor( 9, "long[9]", typeof(long[][][][][][][][][]) );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
    	
	    private void testconstructor( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = Validator_long.Get( n );
		    Assert.AreEqual( n, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }
    	
    	
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor2()  
	    {
		    Validator_long.Get( -1 );
	    }
    	
    	
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor3()  
	    {
		    Validator_long.Get( Validator.MAX_NDIMS+1 );
	    }
    	
	    /** @  */
	    [Test]
	    public void elementvalidator1()  
	    {
		    testelementvalidator( 1, "long[0]", typeof(long) );
		    testelementvalidator( 2, "long[1]", typeof(long[]) );
		    testelementvalidator( 3, "long[2]", typeof(long[][]));
		    testelementvalidator( 4, "long[3]", typeof(long[][][]) );
		    testelementvalidator( 5, "long[4]", typeof(long[][][][]) );
		    testelementvalidator( 6, "long[5]", typeof(long[][][][][]) );
		    testelementvalidator( 7, "long[6]", typeof(long[][][][][][]) );
		    testelementvalidator( 8, "long[7]", typeof(long[][][][][][][]) );
		    testelementvalidator( 9, "long[8]", typeof(long[][][][][][][][]) );
	        Assert.AreEqual( 9, Validator.MAX_NDIMS );
          
	    }
    	
	    private void testelementvalidator( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = (TypeValidator) Validator_long.Get( n ).ElementValidator();
		    Assert.AreEqual( n-1, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }

    	
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void elementvalidator2()  
	    {
		    Validator_long.Get( 0 ).ElementValidator();
	    }
    	
	    /** @  */
	    [Test]
	    public void good_scalar()  
	    {
		    // BYTES
    		
		    testgoodvalue( 0,  (sbyte) 0 );
    		
		    testgoodvalue( 0,  (sbyte) 1 );
		    testgoodvalue( 0,  (sbyte) 64 );
		    testgoodvalue( 0,  SByte.MaxValue );
    		
		    testgoodvalue( 0,  (sbyte) -1 );
		    testgoodvalue( 0,  (sbyte) -64 );
		    testgoodvalue( 0,  SByte.MinValue );

		    // SHORTS
    		
		    testgoodvalue( 0,  (short) 0 );
    		
		    testgoodvalue( 0,  (short) 1 );
		    testgoodvalue( 0,  (short) 64 );
		    testgoodvalue( 0,  (short) SByte.MaxValue );
		    testgoodvalue( 0,  (short) 2222 );
		    testgoodvalue( 0,  Int16.MaxValue );
    		
		    testgoodvalue( 0,  (short) -1 );
		    testgoodvalue( 0,  (short) -64 );
		    testgoodvalue( 0,  (short) SByte.MinValue );
		    testgoodvalue( 0,  (short) -2222 );
		    testgoodvalue( 0,  Int16.MinValue );

		    // INTS
    		
		    testgoodvalue( 0,  0 );
    		
		    testgoodvalue( 0,  1 );
		    testgoodvalue( 0,  64 );
		    testgoodvalue( 0,  (int) SByte.MaxValue );
		    testgoodvalue( 0,  2222 );
		    testgoodvalue( 0,  (int) Int16.MaxValue );
		    testgoodvalue( 0,  33333333 );
		    testgoodvalue( 0,  Int32.MaxValue );
    		
		    testgoodvalue( 0,  -1 );
		    testgoodvalue( 0,  -64 );
		    testgoodvalue( 0,  (int) SByte.MinValue );
		    testgoodvalue( 0,  -2222 );
		    testgoodvalue( 0,  (int) Int16.MinValue );
		    testgoodvalue( 0,  -33333333 );
		    testgoodvalue( 0,  Int32.MinValue );

		    // LONGS
    		
		    testgoodvalue( 0,  (long) 0 );
    		
		    testgoodvalue( 0,  (long) 1 );
		    testgoodvalue( 0,  (long) 64 );
		    testgoodvalue( 0,  (long) SByte.MaxValue );
		    testgoodvalue( 0,  (long) 2222 );
		    testgoodvalue( 0,  (long) Int16.MaxValue );
		    testgoodvalue( 0,  (long) 33333333 );
		    testgoodvalue( 0,  (long) Int32.MaxValue );
		    testgoodvalue( 0,  4444444444444444L );
		    testgoodvalue( 0,  Int64.MaxValue );
    		
		    testgoodvalue( 0,  (long) -1 );
		    testgoodvalue( 0,  (long) -64 );
		    testgoodvalue( 0,  (long) SByte.MinValue );
		    testgoodvalue( 0,  (long) -2222 );
		    testgoodvalue( 0,  (long) Int16.MinValue );
		    testgoodvalue( 0,  (long) -33333333 );
		    testgoodvalue( 0,  (long) Int32.MinValue );
		    testgoodvalue( 0,  -4444444444444444L );
		    testgoodvalue( 0,  Int64.MinValue );
	    }
    	
	    /** @  */
	    [Test]
	    public void good_array()  
	    {
		    testgoodvalue( 1,  new long[] {} );
		    testgoodvalue( 2,  new long[][] {} );
		    testgoodvalue( 3,  new long[][][] {} );
		    testgoodvalue( 4,  new long[][][][] {} );
		    testgoodvalue( 5,  new long[][][][][] {} );
		    testgoodvalue( 6,  new long[][][][][][] {} );
		    testgoodvalue( 7,  new long[][][][][][][] {} );
		    testgoodvalue( 8,  new long[][][][][][][][] {} );
		    testgoodvalue( 9,  new long[][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
    	
	    private void testgoodvalue( int n, Object value )
	    {
		    TypeValidator v = Validator_long.Get( n );
		    Assert.IsTrue( v.Validate( value ) );
		    Assert.IsTrue( validateValueOk( v, value ) );
	    }
    	
	    /** @  */
	    [Test]
	    public void bad_scalar()  
	    {
		    testbadvalue( 0, null );
		    testbadvalue( 0, false );
		    testbadvalue( 0, true );
    //		testbadvalue( 0, (byte) 1 ); good!
    //		testbadvalue( 0, (short) 2222 ); good!
    //		testbadvalue( 0, 33333333 ); good!
    //		testbadvalue( 0, 4444444444444444L ); good!
		    testbadvalue( 0, 5.5f );
		    testbadvalue( 0, 6.6 );
		    testbadvalue( 0, "" );
		    testbadvalue( 0, "abc" );
		    testbadvalue( 0, new Object() );
            testbadvalue(0, new StructValue(new XType("abc"), vf));
		    testbadvalue( 0, new DateTime() );

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
		    testbadvalue( 0, new long[] {} );
		    testbadvalue( 1, new long[][] {} );
		    testbadvalue( 2, new long[][][] {} );
		    testbadvalue( 3, new long[][][][] {} );
		    testbadvalue( 4, new long[][][][][] {} );
		    testbadvalue( 5, new long[][][][][][] {} );
		    testbadvalue( 6, new long[][][][][][][] {} );
		    testbadvalue( 7, new long[][][][][][][][] {} );
		    testbadvalue( 8, new long[][][][][][][][][] {} );
		    testbadvalue( 9, new long[][][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );

		    testbadvalue( 2, new long[] {} );
		    testbadvalue( 3, new long[][] {} );
		    testbadvalue( 4, new long[][][] {} );
		    testbadvalue( 5, new long[][][][] {} );
		    testbadvalue( 6, new long[][][][][] {} );
		    testbadvalue( 7, new long[][][][][][] {} );
		    testbadvalue( 8, new long[][][][][][][] {} );
		    testbadvalue( 9, new long[][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }

	    private void testbadvalue( int n, Object value )
	    {
		    TypeValidator v = Validator_long.Get( n );
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
		    catch ( Exception  )
		    {
              
			    return false;
		    }
	    }

    }
}
