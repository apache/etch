///$Id$
///
///Created by Champakesan, Badri Narayanan on Jun 8, 2007.
///
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.


using System;
using NUnit.Framework;

namespace Etch.Msg.Test
{

    [TestFixture]
    public class TestIdName
    {
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