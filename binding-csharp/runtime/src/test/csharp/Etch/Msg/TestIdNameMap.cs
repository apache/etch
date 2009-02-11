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
using System.Collections.Generic;
using NUnit.Framework;

namespace Org.Apache.Etch.Bindings.Csharp.Msg
{

    public class MyIdNameMap<T> : IdNameMap<IdName>
    {
        public override IdName MakeNew(string name)
        {
            return new IdName( name );
        }
    }

    [TestFixture]
    public class TestIdNameMap
    {
        [TestFixtureSetUp]
        public void First()
        {
            Console.WriteLine();
            Console.Write( "TestIdNameMap" );
        }

        [SetUp] // Before
        public void Init()
        {
            map = new MyIdNameMap<IdName>();
            a = map.Add(new IdName(1, "a"));
            b = map.Add(new IdName(2, "b"));
        }

        private IdNameMap<IdName> map;

        private IdName a;

        private IdName b;

        [Test]
	    public void Check_a()
	    {
		    Assert.AreEqual( 2, map.Size() );
		    Check( "a", 1, a );
		    Assert.AreEqual( 2, map.Size() );
	    }
    	
	    private void Check( String name, int id, IdName idName )
	    {
		    IdName x = map.Get( id );
		    Assert.IsNotNull( x );
		    Assert.AreSame( idName, x );
		    Assert.AreEqual( id, x.Id );
		    Assert.AreEqual( name, x.Name );
    		
		    x = map.Get( name );
		    Assert.IsNotNull( x );
		    Assert.AreSame( idName, x );
		    Assert.AreEqual( id, x.Id );
		    Assert.AreEqual( name, x.Name );
	    }
    	
	    
	    [Test]
	    public void Check_b()
	    {
		    Assert.AreEqual( 2, map.Size() );
		    Check( "b", 2, b );
		    Assert.AreEqual( 2, map.Size() );
	    }
    	
	    
	    [Test]
	    public void Get_3()
	    {
		    Assert.AreEqual( 2, map.Size() );
		    IdName x = map.Get( 3 );
		    Assert.IsNull( x );
		    Assert.AreEqual( 2, map.Size() );
	    }
    	
	    
	    [Test]
	    public void Get_3_locked()
	    {
		    Assert.AreEqual( 2, map.Size() );
		    map.Lock();
		    IdName x = map.Get( 3 );
		    Assert.IsNull( x );
		    Assert.AreEqual( 2, map.Size() );
	    }
    	
	    
	    [Test]
	    public void Get_c()
	    {
		    Assert.AreEqual( 2, map.Size() );
		    IdName c = map.Get( "c" );
		    Check( "c", 352988318, c );
		    Assert.AreEqual( 3, map.Size() );
	    }
    	
	    
	    [Test]
	    public void Get_d_locked()
	    {
		    Assert.AreEqual( 2, map.Size() );
		    map.Lock();
		    IdName d = map.Get( "d" );
		    Assert.IsNull( d );
		    Assert.AreEqual( 2, map.Size() );
	    }

	    
	    [Test]
        [ ExpectedException ( typeof( Exception ) ) ]
	    public void Add_3_c_locked()
	    {
		    Assert.AreEqual( 2, map.Size() );
		    map.Lock();
		    map.Add( new IdName( 3, "c" ) );
		    Assert.AreEqual( 2, map.Size() );
	    }
    	
	    
	    [Test]
        [ ExpectedException( typeof( ArgumentException ) ) ]
	    public void Add_id_coll()
	    {
		    Assert.AreEqual( 2, map.Size() );
		    map.Add( new IdName( 1, "c" ) );
		    Assert.AreEqual( 2, map.Size() );
	    }
    	
	    
	    [Test]
        [ ExpectedException( typeof( ArgumentException ) ) ]
	    public void Add_name_coll()
	    {
		    Assert.AreEqual( 2, map.Size() );
		    map.Add( new IdName( 3, "a" ) );
		    Assert.AreEqual( 2, map.Size() );
	    }
    	
	    
	    [Test]
        [ ExpectedException( typeof( ArgumentException ) ) ]
	    public void Add_id_name_coll()
	    {
		    Assert.AreEqual( 2, map.Size() );
		    map.Add( new IdName( 1, "a" ) );
		    Assert.AreEqual( 2, map.Size() );
	    }
    	
	    
	    [Test]
	    public void Values1()
	    {
		    List<IdName> actual = map.Values();
		    Assert.AreEqual( 2, actual.Count );
		    List<IdName> expected = new List<IdName>();
		    expected.Add( new IdName( 1, "a" ) );
		    expected.Add( new IdName( 2, "b" ) );
		    Assert.AreEqual( expected, actual );
    		
		    map.Get( "c" );
    		
		    actual = map.Values();
		    Assert.AreEqual( 3, actual.Count );
		    expected = new List<IdName>();
		    expected.Add( new IdName( 1, "a" ) );
		    expected.Add( new IdName( 2, "b" ) );
		    expected.Add( new IdName( "c" ) );
		    Assert.AreEqual( expected, actual );
	    }

        [Test]
        public void Values2()
        {
            Assert.AreNotSame(map.Values(),map.Values());
        }
    }
}