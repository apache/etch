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

namespace Org.Apache.Etch.Bindings.Csharp.Msg
{

    [TestFixture]
    public class TestIdName
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestIdName" );
        }

        [Test]
        public void Id()
        {
            Assert.AreEqual(1, new IdName(1, "foo").Id);
            Assert.AreEqual(1, new IdName(1, "bar").Id);
            Assert.AreEqual(2, new IdName(2, "foo").Id);
            Assert.AreEqual(2, new IdName(2, "bar").Id);
        }

        [Test]
        public void Name()
        {
            Assert.AreEqual("foo", new IdName(1, "foo").Name);
            Assert.AreEqual("bar", new IdName(1, "bar").Name);
            Assert.AreEqual("foo", new IdName(2, "foo").Name);
            Assert.AreEqual("bar", new IdName(2, "bar").Name);
        }

        [Test]
        public void XToString() 
        {
            Assert.AreEqual( "foo(1)", new IdName( 1, "foo" ).ToString() );
            Assert.AreEqual( "bar(1)", new IdName( 1, "bar" ).ToString() );
            Assert.AreEqual( "foo(2)", new IdName( 2, "foo" ).ToString() );
            Assert.AreEqual( "bar(2)", new IdName( 2, "bar" ).ToString() );
        }

        [Test]
        public void EqualsObject()
        {
            Assert.IsTrue( new IdName( 1, "foo" ).Equals( new IdName( 1, "foo" ) ) );
            Assert.IsTrue( new IdName( 1, "bar" ).Equals( new IdName( 1, "bar" ) ) );
            Assert.IsTrue( new IdName( 2, "foo" ).Equals( new IdName( 2, "foo" ) ) );
            Assert.IsTrue( new IdName( 2, "bar" ).Equals( new IdName( 2, "bar" ) ) );

            Assert.IsFalse( new IdName( 1, "foo" ).Equals( new IdName( 2, "foo" ) ) ); // id changed
            Assert.IsFalse( new IdName( 1, "foo" ).Equals( new IdName( 1, "bar" ) ) ); // name changed
            Assert.IsFalse( new IdName( 1, "foo" ).Equals( new IdName( 2, "bar" ) ) ); // id and name changed

            Assert.IsFalse( new IdName( 1, "foo" ).Equals( null ) );
            Assert.IsFalse( new IdName( 1, "foo" ).Equals( 1 ) );
            Assert.IsFalse( new IdName( 1, "foo" ).Equals( true ) );
        }

        [Test]
        public void HashCodeObject() 
        {
            // you cannot really predict when HashCode will not be the same. the
            // rule is that it must be the same when the objects would compare equal.
            Assert.AreEqual( new IdName( 1, "foo" ).GetHashCode(), new IdName( 1, "foo" ).GetHashCode() );
            Assert.AreEqual( new IdName( 2, "foo" ).GetHashCode(), new IdName( 2, "foo" ).GetHashCode() );
            Assert.AreEqual( new IdName( 1, "bar" ).GetHashCode(), new IdName( 1, "bar" ).GetHashCode() );
            Assert.AreEqual( new IdName( 2, "bar" ).GetHashCode(), new IdName( 2, "bar" ).GetHashCode() );
        }

        [Test]
        public void Hash()
        {
            Assert.AreEqual(5381, IdName.Hash(""));
            Assert.AreEqual(352988316, IdName.Hash("a"));
            Assert.AreEqual(1511848646, IdName.Hash("ab"));
            Assert.AreEqual(669497117, IdName.Hash("abc"));
            Assert.AreEqual(-1994190713, IdName.Hash("abcd"));
            Assert.AreEqual(-802680418, IdName.Hash("abcde"));
            Assert.AreEqual(1266308680, IdName.Hash("abcdef"));
            Assert.AreEqual(-379372513, IdName.Hash("abcdefg"));
            Assert.AreEqual(-1416967159, IdName.Hash("abcdefgh"));
            Assert.AreEqual(53556896, IdName.Hash("abcdefghi"));
            Assert.AreEqual(-4427318, IdName.Hash("abcdefghij"));
        }
    }
}