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
using Org.Apache.Etch.Bindings.Csharp.Support;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Msg
{
    [TestFixture]
    public class TestComboValidator
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestComboValidator" );
        }

        [Test]
        public void Test()
        {
            Test(0, "(int[0] OR boolean[0])", true, 123456789012L);
            Test(0, "(int[0] OR boolean[0])", false, -123456789012L);
		//    Test( 0, "(int[0] OR boolean[0])", Etch.Transport.Fmt.TypeCode.INT1, 0, (long) 3 );
		    Test( 0, "(int[0] OR boolean[0])",  1, (float) 4 );
		    Test( 0, "(int[0] OR boolean[0])",  -1, (double) 5 );
		    Test( 0, "(int[0] OR boolean[0])",  127, "abc" );
		    Test( 0, "(int[0] OR boolean[0])",  -128, new Object() );
		    Test( 0, "(int[0] OR boolean[0])",  32767, new int[] {} );
		    Test( 0, "(int[0] OR boolean[0])",  -32768, new Boolean[] {} );
		    Test( 0, "(int[0] OR boolean[0])",  1000000000, new DateTime() );
            Test( 0, "(int[0] OR boolean[0])",  -1000000000, typeof( int ) );
		    Test( 1, "(int[1] OR boolean[1])",  new int[] {}, 0 );
		    Test( 1, "(int[1] OR boolean[1])", new Boolean[] {}, true );
        }

        private void Test( int n, String s, Object good, Object bad )
        {
            Validator_int v1 = Validator_int.Get( n );
            Validator_boolean v2 = Validator_boolean.Get( n );
            ComboValidator v = new ComboValidator( v1, v2 );

            Assert.AreEqual( s, v.ToString() );
            Assert.IsTrue( v.Validate( good ) );
            Assert.IsFalse( v.Validate( bad ) );
            
            if ( n > 0 )
            {
                v.ElementValidator();
            }
        }

        [Test]
	    public void Test2()  
	    {
		    Validator v1 = Validator_int.Get( 1 );
		    Validator v2 = Validator_boolean.Get( 0 );
		    Validator v = new ComboValidator( v1, v2 );

		    Assert.IsTrue( v.Validate( new int[] {} ) );
		    Assert.IsTrue( v.Validate( true ) );
		    Assert.IsTrue( v.Validate( false ) );
		    Assert.IsFalse( v.Validate( 0 ) );
    		
		    v = v.ElementValidator();

		    Assert.IsFalse( v.Validate( new int[] {} ) );
		    Assert.IsFalse( v.Validate( true ) );
		    Assert.IsFalse( v.Validate( false ) );
		    Assert.IsTrue( v.Validate( 0 ) );
	    }
    	
	    [Test]
	    public void Test3()  
	    {
		    Validator v1 = Validator_boolean.Get( 0 );
		    Validator v2 = Validator_int.Get( 1 );
		    Validator v = new ComboValidator( v1, v2 );

		    Assert.IsTrue( v.Validate( new int[] {} ) );
		    Assert.IsTrue( v.Validate( true ) );
		    Assert.IsTrue( v.Validate( false ) );
		    Assert.IsFalse( v.Validate( 0 ) );
    		
		    v = v.ElementValidator();

		    Assert.IsFalse( v.Validate( new int[] {} ) );
		    Assert.IsFalse( v.Validate( true ) );
		    Assert.IsFalse( v.Validate( false ) );
		    Assert.IsTrue( v.Validate( 0 ) );
	    }


        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
	    public void Test4()  
	    {
		    Validator v1 = Validator_boolean.Get( 0 );
		    Validator v2 = Validator_int.Get( 1 );
		    Validator v = new ComboValidator( v1, v2 );

		    Assert.IsTrue( v.Validate( new int[] {} ) );
		    Assert.IsTrue( v.Validate( true ) );
		    Assert.IsTrue( v.Validate( false ) );
		    Assert.IsFalse( v.Validate( 0 ) );
    		
		    v = v.ElementValidator();

		    Assert.IsFalse( v.Validate( new int[] {} ) );
		    Assert.IsFalse( v.Validate( true ) );
		    Assert.IsFalse( v.Validate( false ) );
		    Assert.IsTrue( v.Validate( 0 ) );
    		
		    v = v.ElementValidator();
	    }
    	
	    [Test]
        [ExpectedException(typeof(ArgumentException))]
	    public void Test5()  
	    {
		    Validator v1 = Validator_boolean.Get( 1 );
		    Validator v2 = Validator_int.Get( 1 );
		    Validator v = new ComboValidator( v1, v2 );

		    Assert.IsTrue( v.Validate( new Boolean[] {} ) );
		    Assert.IsTrue( v.Validate( new int[] {} ) );
    		
		    v = v.ElementValidator();

		    Assert.IsTrue( v.Validate( true ) );
		    Assert.IsTrue( v.Validate( false ) );
		    Assert.IsTrue( v.Validate( 0 ) );
    		
		    v = v.ElementValidator();
	    }
    }
}
