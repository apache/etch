using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using Etch.Support;
using Etch.Msg;

namespace Etch.Xml
{
    [TestFixture]
    public class TestDynamicValueFactory
    {
        DynamicValueFactory dvf = new DynamicValueFactory();

        /////////////
        // Methods //
        /////////////

        [Test]
        public void TestNothing()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.nothing" );
            XType myResultType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test._result_nothing" );

            Assert.IsNotNull( myType );
            Assert.IsNotNull( myResultType );
            Assert.AreEqual( myResultType, myType.GetResult() );
            Assert.AreEqual( Validator_long.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName("_messageId") ) );
        }

        [Test]
        public void TestIncr()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.incr" );
            XType myResultType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test._result_incr" );

            Assert.IsNotNull( myType );
            Assert.IsNotNull( myResultType );
            Assert.AreEqual( myResultType, myType.GetResult() );

            // validators
            Assert.AreEqual( Validator_long.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName("_messageId") ) );
            Assert.AreEqual( Validator_int.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "x" ) ) );

            Validator comboVal = new ComboValidator( Validator_int.Get( 0 ), Validator_RuntimeException.Get() );
            Validator returnVal = myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "result" ) );

            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );
            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_inReplyTo" ) ) );
        }

        [Test]
        public void TestSum()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.sum" );
            XType myResultType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test._result_sum" );

            Assert.IsNotNull( myType );
            Assert.IsNotNull( myResultType );
            Assert.AreEqual( myResultType, myType.GetResult() );

            // validators
            Assert.AreEqual( Validator_long.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );
            Assert.AreEqual( Validator_int.Get( 1 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "x" ) ) );

            Validator comboVal = new ComboValidator( Validator_int.Get( 0 ), Validator_RuntimeException.Get() );
            Validator returnVal = myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "result" ) );

            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );
            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_inReplyTo" ) ) );
        }

        [Test]
        public void TestFill()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.fill" );
            XType myResultType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test._result_fill" );

            Assert.IsNotNull( myType );
            Assert.IsNotNull( myResultType );
            Assert.AreEqual( myResultType, myType.GetResult() );

            // validators
            Assert.AreEqual( Validator_long.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );
            Assert.AreEqual( Validator_int.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "n" ) ) );
            Assert.AreEqual( Validator_int.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "x" ) ) );

            Validator comboVal = new ComboValidator( Validator_int.Get( 1 ), Validator_RuntimeException.Get() );
            Validator returnVal = myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "result" ) );

            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );
            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_inReplyTo" ) ) );
        }

        [Test]
        public void TestAlwaysWorks()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.alwaysWorks" );
            XType myResultType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test._result_alwaysWorks" );

            Assert.IsNotNull( myType );
            Assert.IsNotNull( myResultType );
            Assert.AreEqual( myResultType, myType.GetResult() );

            // validators
            Assert.AreEqual( Validator_long.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );

            Validator comboVal = new ComboValidator( Validator_int.Get( 0 ), Validator_RuntimeException.Get() );
            Validator returnVal = myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "result" ) );

            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );
            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_inReplyTo" ) ) );
        }

        [Test]
        public void TestIsTrue()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.isTrue" );
            XType myResultType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test._result_isTrue" );

            Assert.IsNotNull( myType );
            Assert.IsNotNull( myResultType );
            Assert.AreEqual( myResultType, myType.GetResult() );

            // validators
            Assert.AreEqual( Validator_long.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );

            Validator comboVal = new ComboValidator( Validator_boolean.Get( 0 ), Validator_RuntimeException.Get() );
            Validator returnVal = myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "result" ) );

            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );
            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_inReplyTo" ) ) );
        }

        [Test]
        public void TestAlwaysFails()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.alwaysFails" );
            XType myResultType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test._result_alwaysFails" );

            Assert.IsNotNull( myType );
            Assert.IsNull( myResultType );
            Assert.AreEqual( myResultType, myType.GetResult() );

            // validators
            Assert.AreEqual( Validator_long.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );

            Validator comboVal = new ComboValidator( Validator_boolean.Get( 0 ), Validator_RuntimeException.Get() );
        }

        [Test]
        public void TestIsFalse()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.isFalse" );
            XType myResultType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test._result_isFalse" );

            Assert.IsNotNull( myType );
            Assert.IsNotNull( myResultType );
            Assert.AreEqual( myResultType, myType.GetResult() );

            // validators
            Assert.AreEqual( Validator_long.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );

            Validator comboVal = new ComboValidator( Validator_boolean.Get( 0 ), Validator_RuntimeException.Get() );
            Validator returnVal = myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "result" ) );

            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );
            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_inReplyTo" ) ) );
        }

        [Test]
        public void TestM3()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.m3" );
            XType myResultType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test._result_m3" );

            Assert.IsNotNull( myType );
            Assert.IsNotNull( myResultType );
            Assert.AreEqual( myResultType, myType.GetResult() );

            // validators
            Assert.AreEqual( Validator_long.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );

            Validator comboVal = new ComboValidator( Validator_void.Get( 0 ), Validator_RuntimeException.Get() );
            Validator returnVal = myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "result" ) );

            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );
            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_inReplyTo" ) ) );
        }

        [Test]
        public void TestCanM1()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.can_m1" );
            XType myResultType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test._result_can_m1" );

            Assert.IsNotNull( myType );
            Assert.IsNotNull( myResultType );
            Assert.AreEqual( myResultType, myType.GetResult() );

            // validators
            Assert.AreEqual( Validator_long.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );

            Validator comboVal = new ComboValidator( Validator_boolean.Get( 0 ), Validator_RuntimeException.Get() );
            Validator returnVal = myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "result" ) );

            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_messageId" ) ) );
            Assert.AreEqual( comboVal.ToString(), returnVal.ToString() );
            Assert.AreEqual( Validator_long.Get( 0 ), myResultType.GetValidator( DynamicValueFactory.fields.GetByName( "_inReplyTo" ) ) );
        }

        ////////////////
        // Exceptions //
        ////////////////

        [Test]
        public void TestExcp1()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.Excp1" );

            Assert.IsNotNull( myType );

            // validators
            Assert.AreEqual( Validator_string.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "msg" ) ) );
            Assert.AreEqual( Validator_int.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "code" ) ) );
        }
        
        [Test]
        public void TestExcp2()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.Excp2" );
            Assert.IsNotNull( myType );

            Assert.AreEqual( 0, myType.validators.Count );
        }

        ///////////
        // Enums //
        ///////////

        [Test]
        public void TestE1()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.E1" );

            Assert.IsNotNull( myType );

            // validators
            Assert.AreEqual( 3, myType.validators.Count );
            Assert.AreEqual( Validator_boolean.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "A" ) ) );
            Assert.AreEqual( Validator_boolean.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "B" ) ) );
            Assert.AreEqual( Validator_boolean.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "C" ) ) );
        }

        [Test]
        public void TestE2()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.E2" );

            Assert.IsNotNull( myType );

            // validators
            Assert.AreEqual( 4, myType.validators.Count );
            Assert.AreEqual( Validator_boolean.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "A" ) ) );
            Assert.AreEqual( Validator_boolean.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "B" ) ) );
            Assert.AreEqual( Validator_boolean.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "C" ) ) );
            Assert.AreEqual( Validator_boolean.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "D" ) ) );
        }

        /////////////
        // Structs //
        /////////////

        /// <summary>
        /// TODO: Complete this test 
        /// once complex type handling 
        /// is completed
        /// </summary>
        public void TestS1()
        {}

        public void TestS2()
        {
            XType myType = DynamicValueFactory.types.GetByName( "etch.bindings.xml.compiler.test.Test.S2" );

            Assert.IsNotNull( myType );

            // validators
            Assert.AreEqual( 4, myType.validators.Count );
            Assert.AreEqual( Validator_int.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "x" ) ) );
            Assert.AreEqual( Validator_int.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "y" ) ) );
            Assert.AreEqual( Validator_int.Get( 0 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "z" ) ) );
            Assert.AreEqual( Validator_int.Get( 1 ), myType.GetValidator( DynamicValueFactory.fields.GetByName( "w" ) ) );
        }
    }
}
