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
    public class TestValidator_double
    {
        private readonly ValueFactory vf = new DummyValueFactory();

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestValidator_double" );
        }

        [Test]
	    public void constructor1() 
	    {
		    testconstructor( 0, "double[0]", typeof(double) );
		    testconstructor( 1, "double[1]", typeof(double[]) );
		    testconstructor( 2, "double[2]", typeof(double[][]) );
		    testconstructor( 3, "double[3]", typeof(double[][][]) );
		    testconstructor( 4, "double[4]", typeof(double[][][][]) );
		    testconstructor( 5, "double[5]", typeof(double[][][][][]) );
		    testconstructor( 6, "double[6]", typeof(double[][][][][][]) );
		    testconstructor( 7, "double[7]", typeof(double[][][][][][][]) );
		    testconstructor( 8, "double[8]", typeof(double[][][][][][][][]) );
		    testconstructor( 9, "double[9]", typeof(double[][][][][][][][][]) );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
	
	    private void testconstructor( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = Validator_double.Get( n );
		    Assert.AreEqual( n, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }
	
	
	    [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor2() 
	    {
		    Validator_double.Get( -1 );
	    }
	
	    [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void constructor3() 
	    {
		    Validator_double.Get( Validator.MAX_NDIMS+1 );
	    }
	
	
	    [Test]
	    public void ElementValidator1() 
	    {
		    testelementvalidator( 1, "double[0]", typeof(double) );
		    testelementvalidator( 2, "double[1]", typeof(double[]) );
		    testelementvalidator( 3, "double[2]", typeof(double[][]) );
		    testelementvalidator( 4, "double[3]", typeof(double[][][]) );
		    testelementvalidator( 5, "double[4]", typeof(double[][][][]) );
		    testelementvalidator( 6, "double[5]", typeof(double[][][][][]) );
		    testelementvalidator( 7, "double[6]", typeof(double[][][][][][]) );
		    testelementvalidator( 8, "double[7]", typeof(double[][][][][][][]) );
		    testelementvalidator( 9, "double[8]", typeof(double[][][][][][][][]) );

		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
	
	    private void testelementvalidator( int n, String descr, Type expectedClass )
	    {
		    TypeValidator v = (TypeValidator) Validator_double.Get( n ).ElementValidator();
		    Assert.AreEqual( n-1, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }

	
	    [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void elementvalidator2() 
	    {
		    Validator_double.Get( 0 ).ElementValidator();
	    }
	
	
	    [Test]
	    public void good_scalar() 
	    {
		    testgoodvalue( 0,  0.0 );
		    testgoodvalue( 0,  1.1 );
		    testgoodvalue( 0,  -1.1 );
	    }
	
	
	    [Test]
	    public void good_array() 
	    {
		    testgoodvalue( 1,  new double[] {} );
		    testgoodvalue( 2,  new double[][] {} );
		    testgoodvalue( 3,  new double[][][] {} );
		    testgoodvalue( 4,  new double[][][][] {} );
		    testgoodvalue( 5,  new double[][][][][] {} );
		    testgoodvalue( 6,  new double[][][][][][] {} );
		    testgoodvalue( 7,  new double[][][][][][][] {} );
		    testgoodvalue( 8,  new double[][][][][][][][] {} );
		    testgoodvalue( 9,  new double[][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
	
	    private void testgoodvalue( int n, Object value )
	    {
		    TypeValidator v = Validator_double.Get( n );
		   
		    Assert.IsTrue( v.Validate( value ) );
		    Assert.IsTrue( ValidateValueOk( v, value ) );
	    }
	
	
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
		    testbadvalue( 0, 5.5f );
    //		testbadvalue( 0, 6.6 ); good!
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
		    testbadvalue( 0, new double[] {} );
		    testbadvalue( 1, new double[][] {} );
		    testbadvalue( 2, new double[][][] {} );
		    testbadvalue( 3, new double[][][][] {} );
		    testbadvalue( 4, new double[][][][][] {} );
		    testbadvalue( 5, new double[][][][][][] {} );
		    testbadvalue( 6, new double[][][][][][][] {} );
		    testbadvalue( 7, new double[][][][][][][][] {} );
		    testbadvalue( 8, new double[][][][][][][][][] {} );
		    testbadvalue( 9, new double[][][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );

		    testbadvalue( 2, new double[] {} );
		    testbadvalue( 3, new double[][] {} );
		    testbadvalue( 4, new double[][][] {} );
		    testbadvalue( 5, new double[][][][] {} );
		    testbadvalue( 6, new double[][][][][] {} );
		    testbadvalue( 7, new double[][][][][][] {} );
		    testbadvalue( 8, new double[][][][][][][] {} );
		    testbadvalue( 9, new double[][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }

	    private void testbadvalue( int n, Object value )
	    {
		    TypeValidator v = Validator_double.Get( n );
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
