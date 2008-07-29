using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;

namespace Etch.Util.Test
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
