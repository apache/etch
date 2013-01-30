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

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{

    [TestFixture]
    public class TestArrayValue
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestArrayValue" );
        }


        [Test]
	    public void Constructor1()
	    {
		    ArrayValue av = new ArrayValue( new sbyte[0] );
		    Assert.AreEqual( (sbyte) 0, av.Type );
		    Assert.IsNull( av.CustomStructType );
		    Assert.AreEqual( 0, av.Dim );
	    }
    	
	    [Test]
	    public void Constructor2()
	    {
		    XType t1 = new XType( "t1" );
		    ArrayValue av = new ArrayValue( new sbyte[0], (sbyte) 1, t1, 2 );
		    Assert.AreEqual( (sbyte) 1, av.Type );
		    Assert.AreEqual( t1, av.CustomStructType );
		    Assert.AreEqual( 2, av.Dim );
	    }
    	
	    [Test]
	    public void constructor3()
	    {
		    XType t2 = new XType( "t2" );
            ArrayValue av = new ArrayValue( new sbyte[ 0 ], ( sbyte ) 3, t2, 4 );
		    Assert.AreEqual( (byte) 3, av.Type );
		    Assert.AreEqual( t2, av.CustomStructType );
		    Assert.AreEqual( 4, av.Dim );
	    }
    	
	    [Test]
	    public void AddMany1()
	    {
		    DateTime dt = new DateTime();
		    ArrayValue av = new ArrayValue( new Object[] { false, (sbyte) 1, (short) 2, 3, (long) 4, (float) 1.5, 1.6, "abc", null, dt } );
		    Assert.AreEqual( 10, av.Size() );
		    int index = 0;
		    Assert.AreEqual( false, av.Get( index++ ) );
		    Assert.AreEqual( (sbyte) 1, av.Get( index++ ) );
		    Assert.AreEqual( (short) 2, av.Get( index++ ) );
		    Assert.AreEqual( 3, av.Get( index++ ) );
		    Assert.AreEqual( (long) 4, av.Get( index++ ) );
		    Assert.AreEqual( (float) 1.5, av.Get( index++ ) );
		    Assert.AreEqual( 1.6, av.Get( index++ ) );
		    Assert.AreEqual( "abc", av.Get( index++ ) );
		    Assert.AreEqual( null, av.Get( index++ ) );
		    Assert.AreEqual( dt, av.Get( index++ ) );
	    }
    	
	    [Test]
	    public void AddMany2()
	    {
		    DateTime dt = new DateTime();
		    ArrayValue av = new ArrayValue( new Object[] { (float) 1.5, 1.6, "abc", null, dt, false, (sbyte) 1, (short) 2, 3, (long) 4 } );
		    Assert.AreEqual( 10, av.Size() );
		    int index = 0;
		    Assert.AreEqual( (float) 1.5, av.Get( index++ ) );
		    Assert.AreEqual( 1.6, av.Get( index++ ) );
		    Assert.AreEqual( "abc", av.Get( index++ ) );
		    Assert.AreEqual( null, av.Get( index++ ) );
		    Assert.AreEqual( dt, av.Get( index++ ) );
		    Assert.AreEqual( false, av.Get( index++ ) );
		    Assert.AreEqual( (sbyte) 1, av.Get( index++ ) );
		    Assert.AreEqual( (short) 2, av.Get( index++ ) );
		    Assert.AreEqual( 3, av.Get( index++ ) );
		    Assert.AreEqual( (long) 4, av.Get( index++ ) );
	    }
    }
}