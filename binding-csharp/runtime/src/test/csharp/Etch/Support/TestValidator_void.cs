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
    public class TestValidator_void
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestValidator_void" );
        }

        [Test]
        public void Test()
        {
            Test( 0, "void[0]", null, ( sbyte ) 1 );
            Test( 0, "void[0]",  null, ( float ) 1 );
            Test( 0, "void[0]",  null, ( int ) 1 );
            Test( 0, "void[0]",  null, ( short ) 1 );
            Test( 0, "void[0]",  null, ( double ) 1 );
            Test( 0, "void[0]",  null, ( long ) 1 );
            Test( 0, "void[0]",  null, "abc" );
        }

        private void Test( int n, String s,Object good, Object bad )
        {
            Validator_void v = Validator_void.Get( n );
            Assert.IsTrue( v.Validate( good ) );
            Assert.IsFalse( v.Validate( bad ) );
          
        }


        [Test]
        [ExpectedException( typeof( ArgumentException ) )]
        public void TestGetNeg1()
        {
            Validator_void.Get( -1 );
        }


        [Test]
        public void TestGetNeg2()
        {
            Assert.IsNull( Validator_void.Get( 0 ).ElementValidator() );
        }


        [Test]
        [ExpectedException( typeof( ArgumentException ) )]
        public void TestGetMaxPlusOne()
        {
            Validator_void.Get( Validator.MAX_NDIMS + 1 );
        }
    }
}
