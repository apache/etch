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
using Org.Apache.Etch.Bindings.Csharp.Support;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Msg
{

    [TestFixture]
    public class TestXType
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestXType" );
        }

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
        public void TestGetValidator() 
	    {
		    XType t = new XType( "t" );
		    Field f = new Field( "f" );
		    Field g = new Field( "g" );
		    Validator v = Validator_int.Get( 0 );
    		
		    t.PutValidator( f, v );

		    Assert.AreEqual( v, t.GetValidator( f ) );
		    Assert.IsNull( t.GetValidator( g ) );
	    }

        [Test]
	    public void TestPutValidatorNull() 
	    {
		    XType t = new XType( "t" );
		    Field f = new Field( "f" );
		    Validator v = Validator_int.Get( 0 );

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