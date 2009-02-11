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
    public class TestValidator_Boolean
    {
        private readonly ValueFactory vf = new DummyValueFactory();

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestValidator_Boolean" );
        }

        [Test]
	    public void Constructor1()  
	    {
		    TestConstructor(0,"boolean[0]",typeof(Boolean));
            TestConstructor(1, "boolean[1]", typeof(Boolean[]));
            TestConstructor(2, "boolean[2]", typeof(Boolean[][]));
            TestConstructor(3, "boolean[3]", typeof(Boolean[][][]));
            TestConstructor(4, "boolean[4]", typeof(Boolean[][][][]));
            TestConstructor(5, "boolean[5]", typeof(Boolean[][][][][]));
            TestConstructor(6, "boolean[6]", typeof(Boolean[][][][][][]));
            TestConstructor(7, "boolean[7]", typeof(Boolean[][][][][][][]));
            TestConstructor(8, "boolean[8]", typeof(Boolean[][][][][][][][]));
            TestConstructor(9, "boolean[9]", typeof(Boolean[][][][][][][][][]));
            Assert.AreEqual(9,Validator.MAX_NDIMS);
	    }

        private void TestConstructor(int n, string descr, Type expectedClass)
        {
            Validator_boolean v = Validator_boolean.Get(n);
            Assert.AreEqual(n, v.GetNDims());
            Assert.AreSame(expectedClass,v.GetExpectedClass());
            Assert.AreEqual(descr, v.ToString());            
        }
 
	    [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
	    public void Constructor2()
	    {		
		    Validator_boolean.Get( -1 );
	    }

        [Test]
        [ExpectedException(typeof(ArgumentOutOfRangeException))]
        public void Constructor3()
        {
            Validator_boolean.Get(Validator.MAX_NDIMS + 1);
        }

        [Test]
        public void ElementValidator1() 
	    {
		    TestElementValidator( 1, "boolean[0]", typeof(Boolean)  );
		    TestElementValidator( 2, "boolean[1]", typeof(Boolean[])  );
		    TestElementValidator( 3, "boolean[2]", typeof(Boolean[][])  );
		    TestElementValidator( 4, "boolean[3]", typeof(Boolean[][][])  );
		    TestElementValidator( 5, "boolean[4]", typeof(Boolean[][][][])  );
		    TestElementValidator( 6, "boolean[5]", typeof(Boolean[][][][][])  );
		    TestElementValidator( 7, "boolean[6]", typeof(Boolean[][][][][][])  );
		    TestElementValidator( 8, "boolean[7]", typeof(Boolean[][][][][][][])  );
		    TestElementValidator( 9, "boolean[8]", typeof(Boolean[][][][][][][][])  );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }

        private void TestElementValidator( int n, String descr, Type expectedClass )
	    {
		    Validator_boolean v = (Validator_boolean) Validator_boolean.Get( n ).ElementValidator();
		    Assert.AreEqual( n-1, v.GetNDims() );
		    Assert.AreSame( expectedClass, v.GetExpectedClass() );
		    Assert.AreEqual( descr, v.ToString() );
	    }


        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void ElementValidator2  ()
	    {		
		    Validator_boolean.Get( 0 ).ElementValidator();
        }

       
	    [Test]
	    public void Good_Scalar() 
	    {
		    TestGoodValue( 0,  false );
		    TestGoodValue( 0, true );
	    }
	
	
	    [Test]
	    public void Good_Array() 
	    {
		    TestGoodValue( 1, new Boolean[] {} );
		    TestGoodValue( 2,  new Boolean[][] {} );
		    TestGoodValue( 3,  new Boolean[][][] {} );
		    TestGoodValue( 4,  new Boolean[][][][] {} );
		    TestGoodValue( 5,  new Boolean[][][][][] {} );
		    TestGoodValue( 6,  new Boolean[][][][][][] {} );
		    TestGoodValue( 7,  new Boolean[][][][][][][] {} );
		    TestGoodValue( 8,  new Boolean[][][][][][][][] {} );
		    TestGoodValue( 9,  new Boolean[][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );
	    }
	
	    private void TestGoodValue( int n,  Object value )
	    {
		    TypeValidator v = Validator_boolean.Get( n );
		   
		    Assert.IsTrue( v.Validate( value ) );
		    Assert.IsTrue( ValidateValueOk( v, value ) );
	    }
	
	
	    [Test]
	    public void bad_scalar() 
	    {
		    TestBadValue( 0, null );
		    TestBadValue( 0, (byte) 1 );
		    TestBadValue( 0, (short) 2222 );
		    TestBadValue( 0, 33333333 );
		    TestBadValue( 0, 4444444444444444L );
		    TestBadValue( 0, 5.5f );
		    TestBadValue( 0, 6.6 );
		    TestBadValue( 0, "" );
		    TestBadValue( 0, "abc" );
		    TestBadValue( 0, new Object() );
            TestBadValue(0, new StructValue(new XType("abc"), vf));
		    TestBadValue( 0, new DateTime() );

		    TestBadValue( 1, null );
		    TestBadValue( 1, false );
		    TestBadValue( 1, true );
		    TestBadValue( 1, (byte) 1 );
		    TestBadValue( 1, (short) 2222 );
		    TestBadValue( 1, 33333333 );
		    TestBadValue( 1, 4444444444444444L );
		    TestBadValue( 1, 5.5f );
		    TestBadValue( 1, 6.6 );
		    TestBadValue( 1, "" );
		    TestBadValue( 1, "abc" );
		    TestBadValue( 1, new Object() );
            TestBadValue(1, new StructValue(new XType("abc"), vf));
		    TestBadValue( 1, new DateTime() );
	    }
	
	
	    [Test]
	    public void bad_array() 
	    {
		    TestBadValue( 0, new Boolean[] {} );
		    TestBadValue( 1, new Boolean[][] {} );
		    TestBadValue( 2, new Boolean[][][] {} );
		    TestBadValue( 3, new Boolean[][][][] {} );
		    TestBadValue( 4, new Boolean[][][][][] {} );
		    TestBadValue( 5, new Boolean[][][][][][] {} );
		    TestBadValue( 6, new Boolean[][][][][][][] {} );
		    TestBadValue( 7, new Boolean[][][][][][][][] {} );
		    TestBadValue( 8, new Boolean[][][][][][][][][] {} );
		    TestBadValue( 9, new Boolean[][][][][][][][][][] {} );
		    Assert.AreEqual( 9, Validator.MAX_NDIMS );

		    TestBadValue( 2, new Boolean[] {} );
		    TestBadValue( 3, new Boolean[][] {} );
		    TestBadValue( 4, new Boolean[][][] {} );
		    TestBadValue( 5, new Boolean[][][][] {} );
		    TestBadValue( 6, new Boolean[][][][][] {} );
		    TestBadValue( 7, new Boolean[][][][][][] {} );
		    TestBadValue( 8, new Boolean[][][][][][][] {} );
		    TestBadValue( 9, new Boolean[][][][][][][][] {} );
            Assert.AreEqual(9, Validator.MAX_NDIMS);
	    }

	    private void TestBadValue( int n, Object value )
	    {
		    TypeValidator v = Validator_boolean.Get( n );
		    Assert.IsFalse( v.Validate( value ) );
		    Assert.IsFalse( ValidateValueOk( v, value ) );
	    }

	    private bool ValidateValueOk( Validator v, Object value )
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
