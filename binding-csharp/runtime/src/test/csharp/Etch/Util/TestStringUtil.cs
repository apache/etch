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
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    
    [TestFixture]
    public class TestStringUtil
    {

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestStringUtil" );
        }


        #region LEFT SPLIT TESTS

        [Test]
        public void TestSimpleLeftSplit()
        {
            String test = "Hello: World";
            String[] s = StringUtil.LeftSplit( test, ':' );
            Assert.AreEqual( "Hello", s[ 0 ] );
            Assert.AreEqual( " World", s[ 1 ] );
        }

        [Test]
        public void TestMultipleLeftSplit()
        {
            String test = "Hello: World: Second";
            String[] s = StringUtil.LeftSplit( test, ':' );
            Assert.AreEqual( "Hello", s[ 0 ] );
            Assert.AreEqual( " World: Second", s[ 1 ] );
        }

        [Test]
        public void TestNoDelimsLeftSplit()
        {
            String test = "Hello World";
            String[] s = StringUtil.LeftSplit( test, ':' );
            Assert.IsNull( s );
        }

        #endregion

        #region RIGHT SPLIT TESTS

        [Test]
        public void TestSimpleRightSplit()
        {
            String test = "Hello: World";
            String[] s = StringUtil.RightSplit( test, ':' );
            Assert.AreEqual( "Hello", s[ 0 ] );
            Assert.AreEqual( " World", s[ 1 ] );
        }

        [Test]
        public void TestMultipleRightSplit()
        {
            String test = "Hello: World: Second";
            String[] s = StringUtil.RightSplit( test, ':' );
            Assert.AreEqual( "Hello: World", s[ 0 ] );
            Assert.AreEqual( " Second", s[ 1 ] );
        }

        [Test]
        public void TestNoDelimsRightSplit()
        {
            String test = "Hello World";
            String[] s = StringUtil.RightSplit( test, ':' );
            Assert.IsNull( s );
        }

        #endregion

        #region TOHEX TESTS

        [Test]
        public void TestToHex_validArgs()
        {
            Assert.AreEqual( '0', StringUtil.ToHex( 0 ) );
            Assert.AreEqual( '1', StringUtil.ToHex( 1 ) );
            Assert.AreEqual( '2', StringUtil.ToHex( 2 ) );
            Assert.AreEqual( '3', StringUtil.ToHex( 3 ) );
            Assert.AreEqual( '4', StringUtil.ToHex( 4 ) );
            Assert.AreEqual( '5', StringUtil.ToHex( 5 ) );
            Assert.AreEqual( '6', StringUtil.ToHex( 6 ) );
            Assert.AreEqual( '7', StringUtil.ToHex( 7 ) );
            Assert.AreEqual( '8', StringUtil.ToHex( 8 ) );
            Assert.AreEqual( '9', StringUtil.ToHex( 9 ) );
            Assert.AreEqual( 'a', StringUtil.ToHex( 10 ) );
            Assert.AreEqual( 'b', StringUtil.ToHex( 11 ) );
            Assert.AreEqual( 'c', StringUtil.ToHex( 12 ) );
            Assert.AreEqual( 'd', StringUtil.ToHex( 13 ) );
            Assert.AreEqual( 'e', StringUtil.ToHex( 14 ) );
            Assert.AreEqual( 'f', StringUtil.ToHex( 15 ) );
        }

        [Test]
        [ExpectedException(typeof(ArgumentException))]
        public void TestToHex_invalidArg()
        {
            StringUtil.ToHex( 16 );
        }

        #endregion

        #region FROMHEX TESTS

        public void TestFromHex()
        {
            Assert.AreEqual( 0, StringUtil.FromHex( '0' ) );
            Assert.AreEqual( 1, StringUtil.FromHex( '1' ) );
            Assert.AreEqual( 2, StringUtil.FromHex( '2' ) );
            Assert.AreEqual( 3, StringUtil.FromHex( '3' ) );
            Assert.AreEqual( 4, StringUtil.FromHex( '4' ) );
            Assert.AreEqual( 5, StringUtil.FromHex( '5' ) );
            Assert.AreEqual( 6, StringUtil.FromHex( '6' ) );
            Assert.AreEqual( 7, StringUtil.FromHex( '7' ) );
            Assert.AreEqual( 8, StringUtil.FromHex( '8' ) );
            Assert.AreEqual( 9, StringUtil.FromHex( '9' ) );
            Assert.AreEqual( 10, StringUtil.FromHex( 'a' ) );
            Assert.AreEqual( 11, StringUtil.FromHex( 'b' ) );
            Assert.AreEqual( 12, StringUtil.FromHex( 'c' ) );
            Assert.AreEqual( 13, StringUtil.FromHex( 'd' ) );
            Assert.AreEqual( 14, StringUtil.FromHex( 'e' ) );
            Assert.AreEqual( 15, StringUtil.FromHex( 'f' ) );
        }

        [Test]
        [ExpectedException(typeof(ArgumentException))]
        public void TestFromHex_invalid()
        {
            StringUtil.FromHex( 'g' );
        }

        #endregion
    }
}
