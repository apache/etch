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
    public class TestField
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestField" );
        }

        [Test]
        public void FieldIntegerString()
        {
            TestMf(1, "one");
            TestMf(2, "two");
            TestMf(3, "three");
        }

        [Test]
        public void FieldString()
        {
            TestMf("one");
            TestMf("two");
            TestMf("three");
        }

        private void TestMf(int id, string name)
        {
            Field mf = new Field(id, name);
            Assert.AreEqual(id, mf.Id);
            Assert.AreEqual(name, mf.Name);
        }

        private void TestMf(string name)
        {
            Field mf = new Field(name);
            Assert.AreEqual(IdName.Hash(name), mf.Id);
            Assert.AreEqual(name, mf.Name);
        }
    }
}