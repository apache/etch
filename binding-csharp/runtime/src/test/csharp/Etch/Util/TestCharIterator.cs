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
    public class TestCharIterator
    {

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestCharIterator" );
        }


        [Test]
        public void TestString1()
        {
            String str1 = "tcp";
            CharIterator ct = new CharIterator( str1 );

            Assert.IsTrue( ct.MoveNext() );
            Assert.AreEqual( 't', ct.Current );
            Assert.IsTrue( ct.MoveNext() );
            Assert.AreEqual( 'c', ct.Current );
            Assert.IsTrue( ct.MoveNext() );
            Assert.AreEqual( 'p', ct.Current );

            Assert.IsFalse( ct.MoveNext() );
        }

        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void TestNullString()
        {
            String str1 = null;
            CharIterator ct = new CharIterator( str1 );
        }

        [Test]
        public void TestEmptyString()
        {
            String str1 = "";
            CharIterator ct = new CharIterator( str1 );

            Assert.IsFalse( ct.MoveNext() );
            
        }
    }
}
