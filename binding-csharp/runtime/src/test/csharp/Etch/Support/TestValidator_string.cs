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
    public class TestValidator_string
    {
        private readonly ValueFactory vf = new DummyValueFactory();

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestValidator_string" );
        }

        [Test]
	public void constructor1()  
	{
		testconstructor( 0, "string[0]", typeof(string) );
		testconstructor( 1, "string[1]", typeof(string[]));
		testconstructor( 2, "string[2]", typeof(string[][]) );
		testconstructor( 3, "string[3]", typeof(string[][][]) );
		testconstructor( 4, "string[4]", typeof(string[][][][]));
		testconstructor( 5, "string[5]", typeof(string[][][][][]) );
		testconstructor( 6, "string[6]", typeof(string[][][][][][]) );
		testconstructor( 7, "string[7]", typeof(string[][][][][][][]) );
		testconstructor( 8, "string[8]", typeof(string[][][][][][][][]) );
		testconstructor( 9, "string[9]", typeof(string[][][][][][][][][]) );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}
	
	private void testconstructor( int n, string descr, Type expectedClass )
	{
		TypeValidator v = Validator_string.Get( n );
		Assert.AreEqual( n, v.GetNDims() );
		Assert.AreSame( expectedClass, v.GetExpectedClass() );
		Assert.AreEqual( descr, v.ToString() );
	}
	
	/** @  */
	[Test]  
    [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	public void constructor2()  
	{
		Validator_string.Get( -1 );
	}
	
	/** @  */
	[Test]  
    [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	public void constructor3()  
	{
		Validator_string.Get( Validator.MAX_NDIMS+1 );
	}
	
	/** @  */
	[Test]
	public void elementvalidator1()  
	{
		testelementvalidator( 1, "string[0]", typeof(string) );
		testelementvalidator( 2, "string[1]", typeof(string[]) );
		testelementvalidator( 3, "string[2]", typeof(string[][]) );
		testelementvalidator( 4, "string[3]", typeof(string[][][]) );
		testelementvalidator( 5, "string[4]", typeof(string[][][][]) );
		testelementvalidator( 6, "string[5]", typeof(string[][][][][]) );
		testelementvalidator( 7, "string[6]", typeof(string[][][][][][]) );
		testelementvalidator( 8, "string[7]", typeof(string[][][][][][][]) );
        testelementvalidator(9,  "string[8]", typeof(string[][][][][][][][]));
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}
	
	private void testelementvalidator( int n, string descr, Type expectedClass )
	{
		TypeValidator v = (TypeValidator) Validator_string.Get( n ).ElementValidator();
		Assert.AreEqual( n-1, v.GetNDims() );
		Assert.AreSame( expectedClass, v.GetExpectedClass() );
		Assert.AreEqual( descr, v.ToString() );
	}

	/** @  */
	[Test]  
    [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	public void elementvalidator2()  
	{
		Validator_string.Get( 0 ).ElementValidator();
	}
	
	/** @  */
	[Test]
	public void good_scalar()  
	{
		testgoodvalue( 0, "" );
		testgoodvalue( 0,  "abc" );
	}
	
	/** @  */
	[Test]
	public void good_array()  
	{
		testgoodvalue( 1,  new string[] {} );
		testgoodvalue( 2,  new string[][] {} );
		testgoodvalue( 3,  new string[][][] {} );
		testgoodvalue( 4,  new string[][][][] {} );
		testgoodvalue( 5,  new string[][][][][] {} );
		testgoodvalue( 6,  new string[][][][][][] {} );
		testgoodvalue( 7,  new string[][][][][][][] {} );
		testgoodvalue( 8,  new string[][][][][][][][] {} );
		testgoodvalue( 9,  new string[][][][][][][][][] {} );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}
	
	private void testgoodvalue( int n,  Object value )
	{
		TypeValidator v = Validator_string.Get( n );
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
		testbadvalue( 0, (byte) 1 );
		testbadvalue( 0, (short) 2222 );
		testbadvalue( 0, 33333333 );
		testbadvalue( 0, 4444444444444444L );
		testbadvalue( 0, 5.5f );
		testbadvalue( 0, 6.6 );
//		testbadvalue( 0, "" ); // good!
//		testbadvalue( 0, "abc" ); // good!
		testbadvalue( 0, new Object() );
		testbadvalue( 0, new StructValue( new XType( "abc" ), vf ) );
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
		testbadvalue( 0, new string[] {} );
		testbadvalue( 1, new string[][] {} );
		testbadvalue( 2, new string[][][] {} );
		testbadvalue( 3, new string[][][][] {} );
		testbadvalue( 4, new string[][][][][] {} );
		testbadvalue( 5, new string[][][][][][] {} );
		testbadvalue( 6, new string[][][][][][][] {} );
		testbadvalue( 7, new string[][][][][][][][] {} );
		testbadvalue( 8, new string[][][][][][][][][] {} );
		testbadvalue( 9, new string[][][][][][][][][][] {} );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );

		testbadvalue( 2, new string[] {} );
		testbadvalue( 3, new string[][] {} );
		testbadvalue( 4, new string[][][] {} );
		testbadvalue( 5, new string[][][][] {} );
		testbadvalue( 6, new string[][][][][] {} );
		testbadvalue( 7, new string[][][][][][] {} );
		testbadvalue( 8, new string[][][][][][][] {} );
		testbadvalue( 9, new string[][][][][][][][] {} );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}

	private void testbadvalue( int n, Object value )
	{
		TypeValidator v = Validator_string.Get( n );
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
