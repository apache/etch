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
    public class TestValidator_byte
    {
        private readonly ValueFactory vf = new DummyValueFactory();

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestValidator_byte" );
        }

        [Test]
        public void Contructor1()
        {
      
            testconstructor( 0, "byte[0]", typeof(sbyte) );
		    testconstructor( 1, "byte[1]", typeof( sbyte[]) );
		    testconstructor( 2, "byte[2]", typeof( sbyte[][] ) );
		    testconstructor( 3, "byte[3]", typeof( sbyte[][][] ) );
		    testconstructor( 4, "byte[4]", typeof( sbyte[][][][] ) );
		    testconstructor( 5, "byte[5]", typeof( sbyte[][][][][] ) );
		    testconstructor( 6, "byte[6]", typeof( sbyte[][][][][][] ) );
		    testconstructor( 7, "byte[7]", typeof( sbyte[][][][][][][] ) );
		    testconstructor( 8, "byte[8]", typeof( sbyte[][][][][][][][] ) );
            testconstructor(9, "byte[9]", typeof(sbyte[][][][][][][][][]));
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
            
        }

        private void testconstructor( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = Validator_byte.Get( n );
		    Assert.AreEqual( n, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }

       
	    [Test] 
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor2() 
	    {
		    Validator_byte.Get( -1 );
	    }
	
	
	    [Test] 
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor3() 
	    {
		    Validator_byte.Get( Validator.MAX_NDIMS+1 );
	    }
	
	
	    [Test]
	    public void elementvalidator1() 
	    {
		    testelementvalidator( 1, "byte[0]", typeof(sbyte) );
		    testelementvalidator( 2, "byte[1]", typeof( sbyte[]) );
		    testelementvalidator( 3, "byte[2]", typeof( sbyte[][] ) );
		    testelementvalidator( 4, "byte[3]", typeof( sbyte[][][] ) );
		    testelementvalidator( 5, "byte[4]", typeof( sbyte[][][][] ) );
		    testelementvalidator( 6, "byte[5]", typeof( sbyte[][][][][] ) );
		    testelementvalidator( 7, "byte[6]", typeof( sbyte[][][][][][] ) );
		    testelementvalidator( 8, "byte[7]", typeof( sbyte[][][][][][][] ) );
		    testelementvalidator( 9, "byte[8]", typeof( sbyte[][][][][][][][] ) );
           
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
	
	    private void testelementvalidator( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = (TypeValidator) Validator_byte.Get( n ).ElementValidator();
		    Assert.AreEqual( n-1, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }

	
	    [Test] 
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void elementvalidator2() 
	    {
		    Validator_byte.Get( 0 ).ElementValidator();
	    }
	
	
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
    		
		    testgoodvalue( 0,  (short) -1 );
		    testgoodvalue( 0,  (short) -64 );
		    testgoodvalue( 0,  (short) SByte.MinValue );

		    // INTS
    		
		    testgoodvalue( 0,  0 );
    		
		    testgoodvalue( 0,  1 );
		    testgoodvalue( 0,  64 );
		    testgoodvalue( 0,  (int) SByte.MaxValue );
    		
		    testgoodvalue( 0,  -1 );
		    testgoodvalue( 0,  -64 );
		    testgoodvalue( 0,  (int) SByte.MinValue );

		    // LONGS
    		
		    testgoodvalue( 0,  (long) 0 );
    		
		    testgoodvalue( 0,  (long) 1 );
		    testgoodvalue( 0,  (long) 64 );
		    testgoodvalue( 0,  (long) SByte.MaxValue );
    		
		    testgoodvalue( 0,  (long) -1 );
		    testgoodvalue( 0,  (long) -64 );
		    testgoodvalue( 0,  (long) SByte.MinValue );
	    }
	
	
	    [Test]
	    public void good_array() 
	    {
		    testgoodvalue( 1,new sbyte[] {} );
		    testgoodvalue( 2,  new sbyte[][] {} );
		    testgoodvalue( 3,  new sbyte[][][] {} );
		    testgoodvalue( 4,  new sbyte[][][][] {} );
		    testgoodvalue( 5,  new sbyte[][][][][] {} );
		    testgoodvalue( 6,  new sbyte[][][][][][] {} );
		    testgoodvalue( 7,  new sbyte[][][][][][][] {} );
		    testgoodvalue( 8,  new sbyte[][][][][][][][] {} );
		    testgoodvalue( 9,  new sbyte[][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
	
	    private void testgoodvalue( int n,Object value )
	    {
		    TypeValidator v = Validator_byte.Get( n );
		    Assert.IsTrue( v.Validate( value ) );
		    Assert.IsTrue( ValidateValueOk( v, value ) );
	    }
	
	    
	    [Test]
	    public void bad_scalar() 
	    {
		    testbadvalue( 0, null );
		    testbadvalue( 0, false );
		    testbadvalue( 0, true );
    //		testbadvalue( 0, (byte) 1 ); good!
		    testbadvalue( 0, (short) 2222 );
		    testbadvalue( 0, 33333333 );
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
	
	
	    [Test]
	    public void bad_array() 
	    {
		    testbadvalue( 0, new sbyte[] {} );
		    testbadvalue( 1, new sbyte[][] {} );
		    testbadvalue( 2, new sbyte[][][] {} );
		    testbadvalue( 3, new sbyte[][][][] {} );
		    testbadvalue( 4, new sbyte[][][][][] {} );
		    testbadvalue( 5, new sbyte[][][][][][] {} );
		    testbadvalue( 6, new sbyte[][][][][][][] {} );
		    testbadvalue( 7, new sbyte[][][][][][][][] {} );
		    testbadvalue( 8, new sbyte[][][][][][][][][] {} );
		    testbadvalue( 9, new sbyte[][][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );

		    testbadvalue( 2, new sbyte[] {} );
		    testbadvalue( 3, new sbyte[][] {} );
		    testbadvalue( 4, new sbyte[][][] {} );
		    testbadvalue( 5, new sbyte[][][][] {} );
		    testbadvalue( 6, new sbyte[][][][][] {} );
		    testbadvalue( 7, new sbyte[][][][][][] {} );
		    testbadvalue( 8, new sbyte[][][][][][][] {} );
		    testbadvalue( 9, new sbyte[][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }

	    private void testbadvalue( int n, Object value )
	    {
	        TypeValidator v = Validator_byte.Get(n);
		    Assert.IsFalse( v.Validate( value ) );
            Assert.IsFalse(ValidateValueOk(v, value));
	    }

	    private bool ValidateValueOk( Validator v, Object value )
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
