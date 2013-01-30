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
        public class TestValidator_float
        {
            private readonly ValueFactory vf = new DummyValueFactory();

            [TestFixtureSetUp]
            public void First()
            {
                Console.WriteLine();
                Console.Write( "TestValidator_float" );
            }

        [Test]
	    public void constructor1() 
	    {
		    testconstructor( 0, "float[0]", typeof(float) );
		    testconstructor( 1, "float[1]", typeof(float[]) );
		    testconstructor( 2, "float[2]", typeof(float[][]));
		    testconstructor( 3, "float[3]", typeof(float[][][]) );
		    testconstructor( 4, "float[4]", typeof(float[][][][]) );
		    testconstructor( 5, "float[5]", typeof(float[][][][][]) );
		    testconstructor( 6, "float[6]", typeof(float[][][][][][]) );
		    testconstructor( 7, "float[7]", typeof(float[][][][][][][]) );
		    testconstructor( 8, "float[8]", typeof(float[][][][][][][][]));
		    testconstructor( 9, "float[9]", typeof(float[][][][][][][][][]) );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
    	
	    private void testconstructor( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = Validator_float.Get( n );
		    Assert.AreEqual( n, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }
    	
    	
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor2() 
	    {
		    Validator_float.Get( -1 );
	    }
    	
	    /** @ */
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor3() 
	    {
		    Validator_float.Get( Validator.MAX_NDIMS+1 );
	    }
    	
	    /** @ */
	    [Test]
	    public void elementvalidator1() 
	    {
            testelementvalidator( 1, "float[0]", typeof(float) );
		    testelementvalidator( 2, "float[1]", typeof(float[]) );
		    testelementvalidator( 3, "float[2]", typeof(float[][]));
		    testelementvalidator( 4, "float[3]", typeof(float[][][]) );
		    testelementvalidator( 5, "float[4]", typeof(float[][][][]) );
		    testelementvalidator( 6, "float[5]", typeof(float[][][][][]) );
		    testelementvalidator( 7, "float[6]", typeof(float[][][][][][]) );
		    testelementvalidator( 8, "float[7]", typeof(float[][][][][][][]) );
		    testelementvalidator( 9, "float[8]", typeof(float[][][][][][][][]));
            Assert.AreEqual( 9, Validator.MAX_NDIMS );
    		
    		
	    }
    	
	    private void testelementvalidator( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = (TypeValidator) Validator_float.Get( n ).ElementValidator();
		    Assert.AreEqual( n-1, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }

    	
	    [Test]  
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void elementvalidator2() 
	    {
		    Validator_float.Get( 0 ).ElementValidator();
	    }
    	
	    /** @ */
	    [Test]
	    public void good_scalar() 
	    {
		    testgoodvalue( 0,0.0f );
		    testgoodvalue( 0,1.1f );
		    testgoodvalue( 0, -1.1f );
	    }
    	
	    /** @ */
	    [Test]
	    public void good_array() 
	    {
		    testgoodvalue( 1,  new float[] {} );
		    testgoodvalue( 2,  new float[][] {} );
		    testgoodvalue( 3,  new float[][][] {} );
		    testgoodvalue( 4,  new float[][][][] {} );
		    testgoodvalue( 5,  new float[][][][][] {} );
		    testgoodvalue( 6,  new float[][][][][][] {} );
		    testgoodvalue( 7,  new float[][][][][][][] {} );
		    testgoodvalue( 8,  new float[][][][][][][][] {} );
		    testgoodvalue( 9,  new float[][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
    	
	    private void testgoodvalue( int n,  Object value )
	    {
		    TypeValidator v = Validator_float.Get( n );
		    Assert.IsTrue( v.Validate( value ) );
		    Assert.IsTrue( validateValueOk( v, value ) );
	    }
    	
	    /** @ */
	    [Test]
	    public void bad_scalar() 
	    {
		    testbadvalue( 0, null );
		    testbadvalue( 0, false );
		    testbadvalue( 0, true );
		    testbadvalue( 0, (byte) 1 );
		    testbadvalue( 0, (short) 2222 );
		    testbadvalue( 0, 33333333 );
		    testbadvalue( 0, 4444444444444444L );
    //		testbadvalue( 0, 5.5f ); good!
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
    	
	    /** @ */
	    [Test]
	    public void bad_array() 
	    {
		    testbadvalue( 0, new float[] {} );
		    testbadvalue( 1, new float[][] {} );
		    testbadvalue( 2, new float[][][] {} );
		    testbadvalue( 3, new float[][][][] {} );
		    testbadvalue( 4, new float[][][][][] {} );
		    testbadvalue( 5, new float[][][][][][] {} );
		    testbadvalue( 6, new float[][][][][][][] {} );
		    testbadvalue( 7, new float[][][][][][][][] {} );
		    testbadvalue( 8, new float[][][][][][][][][] {} );
		    testbadvalue( 9, new float[][][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );

		    testbadvalue( 2, new float[] {} );
		    testbadvalue( 3, new float[][] {} );
		    testbadvalue( 4, new float[][][] {} );
		    testbadvalue( 5, new float[][][][] {} );
		    testbadvalue( 6, new float[][][][][] {} );
		    testbadvalue( 7, new float[][][][][][] {} );
		    testbadvalue( 8, new float[][][][][][][] {} );
		    testbadvalue( 9, new float[][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }

	    private void testbadvalue( int n, Object value )
	    {
		    TypeValidator v = Validator_float.Get( n );
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
