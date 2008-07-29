using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Msg;

namespace Etch.Msg.Test
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
