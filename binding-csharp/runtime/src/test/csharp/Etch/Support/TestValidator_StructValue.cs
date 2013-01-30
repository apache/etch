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
    public class TestValidator_StructValue
    {
        private readonly ValueFactory vf = new DummyValueFactory();

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write("TestValidator_StructValue");
        }

        private XType abc = new XType( "abc" );
	
	/** @  */
	[Test]
	public void constructor1()  
	{
		testconstructor( 0, "StructValue[abc(669497117),0]", typeof(StructValue));
		testconstructor( 1, "StructValue[abc(669497117),1]", typeof(StructValue[]) );
		testconstructor( 2, "StructValue[abc(669497117),2]", typeof(StructValue[][]) );
		testconstructor( 3, "StructValue[abc(669497117),3]", typeof(StructValue[][][]) );
		testconstructor( 4, "StructValue[abc(669497117),4]", typeof(StructValue[][][][]) );
		testconstructor( 5, "StructValue[abc(669497117),5]", typeof(StructValue[][][][][]) );
		testconstructor( 6, "StructValue[abc(669497117),6]", typeof(StructValue[][][][][][]) );
		testconstructor( 7, "StructValue[abc(669497117),7]", typeof(StructValue[][][][][][][]) );
		testconstructor( 8, "StructValue[abc(669497117),8]", typeof(StructValue[][][][][][][][]) );
		testconstructor( 9, "StructValue[abc(669497117),9]", typeof(StructValue[][][][][][][][][]) );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}
	
	private void testconstructor( int n, String descr, Type expectedClass )
	{
		Validator_StructValue v = Validator_StructValue.Get( abc, n );
		Assert.AreEqual( n, v.GetNDims() );
		Assert.AreSame( expectedClass, v.GetExpectedClass() );
		Assert.AreEqual( descr, v.ToString() );
		Assert.AreEqual( abc, v.GetXType() );
	}
	
	
	[Test]  
    [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	public void constructor2()  
	{
		Validator_StructValue.Get( abc, -1 );
	}
	
	/** @  */
	[Test]  
    [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	public void constructor3()  
	{
		Validator_StructValue.Get( abc, Validator.MAX_NDIMS+1 );
	}
	
	/** @  */
	[Test]
	public void elementvalidator1()  
	{
		testelementvalidator( 1, "StructValue[abc(669497117),0]", typeof(StructValue) );
		testelementvalidator( 2, "StructValue[abc(669497117),1]", typeof(StructValue[]) );
		testelementvalidator( 3, "StructValue[abc(669497117),2]", typeof(StructValue[][]) );
		testelementvalidator( 4, "StructValue[abc(669497117),3]", typeof(StructValue[][][]) );
		testelementvalidator( 5, "StructValue[abc(669497117),4]", typeof(StructValue[][][][]) );
		testelementvalidator( 6, "StructValue[abc(669497117),5]", typeof(StructValue[][][][][]) );
		testelementvalidator( 7, "StructValue[abc(669497117),6]", typeof(StructValue[][][][][][]) );
		testelementvalidator( 8, "StructValue[abc(669497117),7]", typeof(StructValue[][][][][][][]) );
		testelementvalidator( 9, "StructValue[abc(669497117),8]", typeof(StructValue[][][][][][][][]) );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}
	
	private void testelementvalidator( int n, String descr, Type expectedClass )
	{
		Validator_StructValue v = (Validator_StructValue) Validator_StructValue.Get( abc, n ).ElementValidator();
		Assert.AreEqual( n-1, v.GetNDims() );
		Assert.AreSame( expectedClass, v.GetExpectedClass() );
		Assert.AreEqual( descr, v.ToString() );
		Assert.AreEqual( abc, v.GetXType() );
	}

	
	[Test]  
    [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	public void elementvalidator2()  
	{
		Validator_StructValue.Get( abc, 0 ).ElementValidator();
	}
	
	
	[Test]
	public void good_scalar()  
	{
        testgoodvalue(0, new StructValue(abc, vf));
	}
	
	/** @  */
	[Test]
	public void good_array()  
	{
		testgoodvalue( 1,  new StructValue[] {} );
		testgoodvalue( 2,  new StructValue[][] {} );
		testgoodvalue( 3,  new StructValue[][][] {} );
		testgoodvalue( 4,  new StructValue[][][][] {} );
		testgoodvalue( 5,  new StructValue[][][][][] {} );
		testgoodvalue( 6,  new StructValue[][][][][][] {} );
		testgoodvalue( 7,  new StructValue[][][][][][][] {} );
		testgoodvalue( 8,  new StructValue[][][][][][][][] {} );
		testgoodvalue( 9,  new StructValue[][][][][][][][][] {} );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}
	
	private void testgoodvalue( int n, Object value )
	{
		TypeValidator v = Validator_StructValue.Get( abc, n );
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
		testbadvalue( 0, "" );
		testbadvalue( 0, "abc" );
		testbadvalue( 0, new Object() );
		testbadvalue( 0, new StructValue( new XType( "def" ), vf ) );
		testbadvalue( 0, new DateTime() );

		testbadvalue( 1, null );
		testbadvalue( 1, false );
		testbadvalue( 1, true );
		testbadvalue( 1, (byte) 1 );
		testbadvalue( 1, (short) 2222 );
		testbadvalue( 1, 333333 );
	    testbadvalue(1, 4444444444444444L);
		testbadvalue( 1, 5.5f );
		testbadvalue( 1, 6.6 );
		testbadvalue( 1, "" );
		testbadvalue( 1, "abc" );
		testbadvalue( 1, new Object() );
	//	testbadvalue( 1, new StructValue( abc ) );
	    testbadvalue(1, new DateTime());
	}
	
	/** @  */
	[Test]
	public void bad_array()  
	{
		testbadvalue( 0, new StructValue[] {} );
		testbadvalue( 1, new StructValue[][] {} );
		testbadvalue( 2, new StructValue[][][] {} );
		testbadvalue( 3, new StructValue[][][][] {} );
		testbadvalue( 4, new StructValue[][][][][] {} );
		testbadvalue( 5, new StructValue[][][][][][] {} );
		testbadvalue( 6, new StructValue[][][][][][][] {} );
		testbadvalue( 7, new StructValue[][][][][][][][] {} );
		testbadvalue( 8, new StructValue[][][][][][][][][] {} );
		testbadvalue( 9, new StructValue[][][][][][][][][][] {} );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );

		testbadvalue( 2, new StructValue[] {} );
		testbadvalue( 3, new StructValue[][] {} );
		testbadvalue( 4, new StructValue[][][] {} );
		testbadvalue( 5, new StructValue[][][][] {} );
		testbadvalue( 6, new StructValue[][][][][] {} );
		testbadvalue( 7, new StructValue[][][][][][] {} );
		testbadvalue( 8, new StructValue[][][][][][][] {} );
		testbadvalue( 9, new StructValue[][][][][][][][] {} );
		Assert.AreEqual( 9, Validator.MAX_NDIMS );
	}

	private void testbadvalue( int n, Object value )
	{
		TypeValidator v = Validator_StructValue.Get( abc, n );
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
