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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    [TestFixture]
    public class TestTypeValidator
    {

        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestTypeValidator" );
        }

        [Test]
        [ExpectedException( typeof( ArgumentOutOfRangeException ) )]
        public void TestValidate()
        {
            new MyTypeValidator( typeof( int ), typeof( int ), -1, "bzzt" );
        }

        [Test]
        public void TestTypeValidator1()
        {
            MyTypeValidator v = new MyTypeValidator( typeof( int ), null, 0, "abc" );
            Assert.AreEqual( typeof( int ), v.GetExpectedClass() );
            Assert.AreEqual( 0, v.GetNDims() );
            Assert.AreEqual( "abc", v.ToString() );
        }

        [Test]
        public void TestTypeValidator2()
        {
            MyTypeValidator v = new MyTypeValidator( typeof(Boolean), null, 0, "def" );
            Assert.AreEqual( typeof( Boolean ), v.GetExpectedClass() );
		    Assert.AreEqual( 0, v.GetNDims() );
		    Assert.AreEqual( "def", v.ToString() );
        }

        [Test]
	    public void TestTypeValidator3()
	    {
		    MyTypeValidator v = new MyTypeValidator( null, typeof(int), 1, "ghi" );
		    Assert.AreEqual( typeof(int[]), v.GetExpectedClass() );
		    Assert.AreEqual( 1, v.GetNDims() );
		    Assert.AreEqual( "ghi", v.ToString() );
	    }

        [Test]
	    public void TestTypeValidator4()
	    {
		    MyTypeValidator v = new MyTypeValidator( null, typeof(Boolean), 1, "jkl" );
		    Assert.AreEqual( typeof(Boolean[]), v.GetExpectedClass() );
		    Assert.AreEqual( 1, v.GetNDims() );
		    Assert.AreEqual( "jkl", v.ToString() );
	    }

        [Test]
	    public void TestToString()
	    {
		    MyTypeValidator v = new MyTypeValidator( typeof(int), typeof(int), 0, "abc" );
		    Assert.AreEqual( "abc", v.ToString() );
    		
		    v = new MyTypeValidator( typeof(int), typeof(int), 0, "def" );
		    Assert.AreEqual( "def", v.ToString() );
    		
		    v = new MyTypeValidator( typeof(int), typeof(int), 0, "ghi" );
		    Assert.AreEqual( "ghi", v.ToString() );
	    }

        [Test]
	    public void TestSilly()
	    {
		  
		    new MyTypeValidator( typeof(int), typeof(int), 0, "ghi" ).ElementValidator();
	    }

        public class MyTypeValidator : TypeValidator
        {
            public MyTypeValidator( Type scalarClass, Type arrayClass, int nDims,
                String descr ) : base ( scalarClass, arrayClass, nDims, descr)
            { }

           

            public override Validator ElementValidator()
            {
                // ignore
                return null;
            }
        }
    }
}
