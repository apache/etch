///$Id$
/// 
///Created by Champakesan, Badri Narayanan on Jun 8, 2007.
/// 
///Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.


using NUnit.Framework;
using System;

namespace Etch.Msg.Test
{

    [TestFixture]
    public class TestXType
    {
        [Test]
        public void TypeIntegerString()
        {
            TestMt(1, "one");
            TestMt(2, "two");
            TestMt(3, "three");
        }

        [Test]
        public void TypeString()
        {
            TestMt("one");
            TestMt("two");
            TestMt("three");
        }

        [Test]
        public void TestRunValidators()
        {
            Boolean b = XType.GetRunValidators();

            XType.SetRunValidators( true );
            Assert.IsTrue( XType.GetRunValidators() );

            XType.SetRunValidators( false );
            Assert.IsFalse( XType.GetRunValidators() );

            XType.SetRunValidators( b );

        }

        [Test]
        public void TestGetValidator() 
	    {
		    XType t = new XType( "t" );
		    Field f = new Field( "f" );
		    Field g = new Field( "g" );
		    Validator v = Validator_int.Get( 0 );
    		
		    t.PutValidator( f, v );
    		
		    XType.SetRunValidators( false );
		    Assert.IsNull( t.GetValidator( f ) );
    		
		    XType.SetRunValidators( true );
		    Assert.AreEqual( v, t.GetValidator( f ) );
    		
		    XType.SetRunValidators( false );
		    Assert.IsNull( t.GetValidator( g ) );
    		
		    XType.SetRunValidators( true );
		    Assert.IsNull( t.GetValidator( g ) );
	    }

        [Test]
	    public void TestPutValidatorNull() 
	    {
		    XType t = new XType( "t" );
		    Field f = new Field( "f" );
		    Validator v = Validator_int.Get( 0 );

		    XType.SetRunValidators( true );
		    Assert.IsNull( t.GetValidator( f ) );
    		
		    t.PutValidator( f, null );
		    Assert.IsNull( t.GetValidator( f ) );
    		
		    t.PutValidator( f, v );
		    Assert.AreEqual( v, t.GetValidator( f ) );
    		
		    t.PutValidator( f, v );
            Assert.AreEqual( typeof( ComboValidator ), ( t.GetValidator( f ) ).GetType() );
    		
		    t.ClearValidator( f );
		    Assert.IsNull( t.GetValidator( f ) );
	    }

        private void TestMt(int id, string name)
        {
            XType mt = new XType(id, name);
            Assert.AreEqual(id, mt.Id);
            Assert.AreEqual(name, mt.Name);
        }

        private void TestMt(string name)
        {
            XType mt = new XType(name);
            Assert.AreEqual(IdName.Hash(name), mt.Id);
            Assert.AreEqual(name, mt.Name);
        }
    }
}