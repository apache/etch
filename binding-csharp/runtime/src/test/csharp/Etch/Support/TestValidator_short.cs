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
    public class TestValidator_short
    {
        private readonly ValueFactory vf = new DummyValueFactory();

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestValidator_short" );
        }

    [Test]
	public void constructor1()  
	{
		testconstructor( 0, "short[0]", typeof(short) );
		testconstructor( 1, "short[1]", typeof(short[]) );
		testconstructor( 2, "short[2]", typeof(short[][]) );
		testconstructor( 3, "short[3]", typeof(short[][][]) );
		testconstructor( 4, "short[4]", typeof(short[][][][]) );
		testconstructor( 5, "short[5]", typeof(short[][][][][]) );
		testconstructor( 6, "short[6]", typeof(short[][][][][][]) );
		testconstructor( 7, "short[7]", typeof(short[][][][][][][]) );
		testconstructor( 8, "short[8]", typeof(short[][][][][][][][]) );
		testconstructor( 9, "short[9]", typeof(short[][][][][][][][][]) );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}
	
	private void testconstructor( int n, String descr, Type expectedClass )
	{
		TypeValidator v = Validator_short.Get( n );
		Assert.AreEqual( n, v.GetNDims() );
		Assert.AreSame( expectedClass, v.GetExpectedClass() );
		Assert.AreEqual( descr, v.ToString() );
	}
	
	/** @  */
	[Test]  
    [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	public void constructor2()  
	{
		Validator_short.Get( -1 );
	}
	
	/** @  */
	[Test]  
    [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	public void constructor3()  
	{
		Validator_short.Get( Validator.MAX_NDIMS+1 );
	}
	
	
	[Test]
	public void elementvalidator1()  
	{
		testelementvalidator( 1, "short[0]", typeof(short) );
		testelementvalidator( 2, "short[1]", typeof(short[]) );
		testelementvalidator( 3, "short[2]", typeof(short[][]) );
		testelementvalidator( 4, "short[3]", typeof(short[][][]) );
		testelementvalidator( 5, "short[4]", typeof(short[][][][]) );
		testelementvalidator( 6, "short[5]", typeof(short[][][][][]) );
		testelementvalidator( 7, "short[6]", typeof(short[][][][][][]) );
		testelementvalidator( 8, "short[7]", typeof(short[][][][][][][]));
		testelementvalidator( 9, "short[8]", typeof(short[][][][][][][][]) );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}
	
	private void testelementvalidator( int n, String descr, Type expectedClass )
	{
		TypeValidator v = (TypeValidator) Validator_short.Get( n ).ElementValidator();
		Assert.AreEqual( n-1, v.GetNDims() );
		Assert.AreSame( expectedClass, v.GetExpectedClass() );
		Assert.AreEqual( descr, v.ToString() );
	}

	/** @  */
	[Test]  [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	public void elementvalidator2()  
	{
		Validator_short.Get( 0 ).ElementValidator();
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
		
		testgoodvalue( 0,  -1 );
		testgoodvalue( 0,  -64 );
		testgoodvalue( 0,  (int) SByte.MinValue );
		testgoodvalue( 0,  -2222 );
		testgoodvalue( 0,  (int) Int16.MinValue );

		// LONGS
		
		testgoodvalue( 0,  (long) 0 );
		
		testgoodvalue( 0,  (long) 1 );
		testgoodvalue( 0,  (long) 64 );
		testgoodvalue( 0,  (long) SByte.MaxValue );
		testgoodvalue( 0,  (long) 2222 );
		testgoodvalue( 0,  (long) Int16.MaxValue );
		
		testgoodvalue( 0,  (long) -1 );
		testgoodvalue( 0,  (long) -64 );
		testgoodvalue( 0,  (long) SByte.MinValue );
		testgoodvalue( 0,  (long) -2222 );
		testgoodvalue( 0,  (long) Int16.MinValue );
	}
	
	/** @  */
	[Test]
	public void good_array()  
	{
		testgoodvalue( 1,  new short[] {} );
		testgoodvalue( 2,  new short[][] {} );
		testgoodvalue( 3,  new short[][][] {} );
		testgoodvalue( 4,  new short[][][][] {} );
		testgoodvalue( 5,  new short[][][][][] {} );
		testgoodvalue( 6,  new short[][][][][][] {} );
		testgoodvalue( 7,  new short[][][][][][][] {} );
		testgoodvalue( 8,  new short[][][][][][][][] {} );
		testgoodvalue( 9,  new short[][][][][][][][][] {} );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}
	
	private void testgoodvalue( int n, Object value )
	{
		TypeValidator v = Validator_short.Get( n );
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
	
	/** @  */
	[Test]
	public void bad_array()  
	{
		testbadvalue( 0, new short[] {} );
		testbadvalue( 1, new short[][] {} );
		testbadvalue( 2, new short[][][] {} );
		testbadvalue( 3, new short[][][][] {} );
		testbadvalue( 4, new short[][][][][] {} );
		testbadvalue( 5, new short[][][][][][] {} );
		testbadvalue( 6, new short[][][][][][][] {} );
		testbadvalue( 7, new short[][][][][][][][] {} );
		testbadvalue( 8, new short[][][][][][][][][] {} );
		testbadvalue( 9, new short[][][][][][][][][][] {} );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );

		testbadvalue( 2, new short[] {} );
		testbadvalue( 3, new short[][] {} );
		testbadvalue( 4, new short[][][] {} );
		testbadvalue( 5, new short[][][][] {} );
		testbadvalue( 6, new short[][][][][] {} );
		testbadvalue( 7, new short[][][][][][] {} );
		testbadvalue( 8, new short[][][][][][][] {} );
		testbadvalue( 9, new short[][][][][][][][] {} );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}

	private void testbadvalue( int n, Object value )
	{
		TypeValidator v = Validator_short.Get( n );
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
		catch ( Exception )
		{
            
			return false;
		}
	}

     
    }
}
