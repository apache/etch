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
        public class TestValidator_int
        {
            private readonly ValueFactory vf = new DummyValueFactory();

            [TestFixtureSetUp]
            public void First()
            {
                Console.WriteLine();
                Console.Write( "TestValidator_int" );
            }

        [Test]
	    public void constructor1()  
	    {
		    testconstructor( 0, "int[0]", typeof(int) );
		    testconstructor( 1, "int[1]", typeof(int[]) );
		    testconstructor( 2, "int[2]", typeof(int[][]) );
		    testconstructor( 3, "int[3]", typeof(int[][][]) );
		    testconstructor( 4, "int[4]", typeof(int[][][][]) );
		    testconstructor( 5, "int[5]", typeof(int[][][][][]) );
		    testconstructor( 6, "int[6]", typeof(int[][][][][][]) );
		    testconstructor( 7, "int[7]", typeof(int[][][][][][][]) );
		    testconstructor( 8, "int[8]", typeof(int[][][][][][][][]) );
		    testconstructor( 9, "int[9]", typeof(int[][][][][][][][][]) );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
    	
	    private void testconstructor( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = Validator_int.Get( n );
		    Assert.AreEqual( n, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }
    	
    	
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor2()  
	    {
		    Validator_int.Get( -1 );
	    }
    	
    	
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor3()  
	    {
		    Validator_int.Get( Validator.MAX_NDIMS+1 );
	    }
    	
    	
	    [Test]
	    public void elementvalidator1()  
	    {
		    testelementvalidator( 1, "int[0]", typeof(int) );
		    testelementvalidator( 2, "int[1]", typeof(int[]) );
		    testelementvalidator( 3, "int[2]", typeof(int[][]) );
		    testelementvalidator( 4, "int[3]", typeof(int[][][]) );
		    testelementvalidator( 5, "int[4]", typeof(int[][][][]) );
		    testelementvalidator( 6, "int[5]", typeof(int[][][][][]) );
		    testelementvalidator( 7, "int[6]", typeof(int[][][][][][]) );
		    testelementvalidator( 8, "int[7]", typeof(int[][][][][][][]) );
		    testelementvalidator( 9, "int[8]", typeof(int[][][][][][][][]) );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
      
	    }
    	
	    private void testelementvalidator( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = (TypeValidator) Validator_int.Get( n ).ElementValidator();
		    Assert.AreEqual( n-1, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }

    	
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void elementvalidator2()  
	    {
		    Validator_int.Get( 0 ).ElementValidator();
	    }
    	
    	
	    [Test]
	    public void good_scalar()  
	    {
    		
		    testgoodvalue( 0,  (sbyte) 0 );
    		
		    testgoodvalue( 0,  (sbyte) 1 );
		    testgoodvalue( 0,  (sbyte) 64 );
		    testgoodvalue( 0,  (sbyte)SByte.MaxValue );
    		
		    testgoodvalue( 0,  (sbyte) -1 );
		    testgoodvalue( 0,  (sbyte) -64 );
		    testgoodvalue( 0,  (sbyte)SByte.MinValue);

		    // SHORTS
    		
		    testgoodvalue( 0,  (short) 0 );
    		
		    testgoodvalue( 0,  (short) 1 );
		    testgoodvalue( 0,  (short) 64 );
		    testgoodvalue( 0,  (short) SByte.MaxValue );
		    testgoodvalue( 0, (short) 2222 );
		    testgoodvalue( 0, Int16.MaxValue );
    		
		    testgoodvalue( 0,  (short) -1 );
		    testgoodvalue( 0,  (short) -64 );
		    testgoodvalue( 0,  (short) SByte.MinValue );
		    testgoodvalue( 0,  (short) -2222 );
		    testgoodvalue( 0, Int16.MinValue );

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
    		
		    testgoodvalue( 0,  (long) -1 );
		    testgoodvalue( 0,  (long) -64 );
		    testgoodvalue( 0,  (long) Byte.MinValue );
		    testgoodvalue( 0,  (long) -2222 );
		    testgoodvalue( 0,  (long) Int16.MinValue );
		    testgoodvalue( 0,  (long) -33333333 );
		    testgoodvalue( 0,  (long) Int32.MinValue );
	    }
    	
    	
	    [Test]
	    public void good_array()  
	    {
		    testgoodvalue( 1,  new int[] {} );
		    testgoodvalue( 2,  new int[][] {} );
		    testgoodvalue( 3,  new int[][][] {} );
		    testgoodvalue( 4,  new int[][][][] {} );
		    testgoodvalue( 5,  new int[][][][][] {} );
		    testgoodvalue( 6,  new int[][][][][][] {} );
		    testgoodvalue( 7,  new int[][][][][][][] {} );
		    testgoodvalue( 8,  new int[][][][][][][][] {} );
		    testgoodvalue( 9,  new int[][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
    	
	    private void testgoodvalue( int n, Object value )
	    {
		    TypeValidator v = Validator_int.Get( n );
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
		    testbadvalue( 0, 4444444444444444L );
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
		    testbadvalue( 0, new int[] {} );
		    testbadvalue( 1, new int[][] {} );
		    testbadvalue( 2, new int[][][] {} );
		    testbadvalue( 3, new int[][][][] {} );
		    testbadvalue( 4, new int[][][][][] {} );
		    testbadvalue( 5, new int[][][][][][] {} );
		    testbadvalue( 6, new int[][][][][][][] {} );
		    testbadvalue( 7, new int[][][][][][][][] {} );
		    testbadvalue( 8, new int[][][][][][][][][] {} );
		    testbadvalue( 9, new int[][][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );

		    testbadvalue( 2, new int[] {} );
		    testbadvalue( 3, new int[][] {} );
		    testbadvalue( 4, new int[][][] {} );
		    testbadvalue( 5, new int[][][][] {} );
		    testbadvalue( 6, new int[][][][][] {} );
		    testbadvalue( 7, new int[][][][][][] {} );
		    testbadvalue( 8, new int[][][][][][][] {} );
		    testbadvalue( 9, new int[][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }

	    private void testbadvalue( int n, Object value )
	    {
		    TypeValidator v = Validator_int.Get( n );
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
