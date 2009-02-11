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
using System.Collections.Generic;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using NUnit.Framework;
using org.apache.etch.tests;
using org.apache.etch.tests.types.Bar;
using org.apache.etch.tests.types.Baz;
using org.apache.etch.tests.types.Foo;

namespace etch.tests
{
     [TestFixture]
    public class TestMixin
    {
      [Test]
	public void Method_testBaz() 
	{
		
		    BazData bazData = null;

		    Baz baz = null;
		    if (baz != null)
		    {
		        baz.baz1(bazData);
		    }
		
	}
	
	
	[Test]
	public void Method_testBazClient() 
	{
		BazData bazData = null;
		
		BazClient bazClient = null;
		if (bazClient != null)
		{
			bazClient.baz1( bazData );
			bazClient.baz3( bazData );
		}
		
		Baz baz = bazClient;
	}
	
	
	[Test]
	public void Method_testBazServer() 
	{
		BazData bazData = null;
		
		BazServer bazServer = null;
		if (bazServer != null)
		{
			bazServer.baz1( bazData );
			bazServer.baz2( bazData );
		}
		
		Baz baz = bazServer;
	}
	
	
	[Test]
	public void Method_testBar() 
	{
		BarData barData = null;
		BazData bazData = null;
		
		Bar bar = null;
		if (bar != null)
		{
			bar.bar1( barData );
			bar.baz1( bazData );
		}
		
		Baz baz = bar;
	}
	
	
	[Test]
	public void Method_testBarClient() 
	{
		BarData barData = null;
		BazData bazData = null;
		
		BarClient barClient = null;
		if (barClient != null)
		{
			barClient.bar1( barData );
			barClient.bar3( barData );
			barClient.baz1( bazData );
			barClient.baz3( bazData );
		}

		BazClient bazClient = barClient;
		Bar bar = barClient;
		Baz baz = barClient;
	}
	
	
	[Test]
	public void Method_testBarServer() 
	{
		BarData barData = null;
		BazData bazData = null;
		
		BarServer barServer = null;
		if (barServer != null)
		{
			barServer.bar1( barData );
			barServer.bar2( barData );
			barServer.baz1( bazData );
			barServer.baz2( bazData );
		}

		BazServer bazServer = barServer;
		Bar bar = barServer;
		Baz baz = barServer;
	}
	
	
	[Test]
	public void Method_testFoo() 
	{
		FooData fooData = null;
		BarData barData = null;
		BazData bazData = null;
		
		Foo foo = null;
		if (foo != null)
		{
			foo.foo1( fooData );
			foo.bar1( barData );
			foo.baz1( bazData );
		}
		
		Bar bar = foo;
		Baz baz = foo;
	}
	
	
	[Test]
	public void testFooClient() 
	{
		FooData fooData = null;
		BarData barData = null;
		BazData bazData = null;
		
		FooClient fooClient = null;
		if (fooClient != null)
		{
			fooClient.foo1( fooData );
			fooClient.foo3( fooData );
			fooClient.bar1( barData );
			fooClient.bar3( barData );
			fooClient.baz1( bazData );
			fooClient.baz3( bazData );
		}
		
		BarClient barClient = fooClient;
		BazClient bazClient = fooClient;
		Foo foo = fooClient;
		Bar bar = fooClient;
		Baz baz = fooClient;
	}
	
	
	[Test]
	public void testFooServer() 
	{
		FooData fooData = null;
		BarData barData = null;
		BazData bazData = null;
		
		FooServer fooServer = null;
		if (fooServer != null)
		{
			fooServer.foo1( fooData );
			fooServer.foo2( fooData );
			fooServer.bar1( barData );
			fooServer.bar2( barData );
			fooServer.baz1( bazData );
			fooServer.baz2( bazData );
		}
		
		BarServer barServer = fooServer;
		BazServer bazServer = fooServer;
		Foo foo = fooServer;
		Bar bar = fooServer;
		Baz baz = fooServer;
	}
	
	
    [Test]
	public void Method_testValueFactoryBazGetTypes() 
	{
        DefaultValueFactory vf = new ValueFactoryBaz("none:");
		List<XType> atypes = (List<XType>)vf.GetTypes();
		
		List<XType> etypes = new List<XType>();


        etypes.Add(vf.get_mt__Etch_RuntimeException());
        etypes.Add(vf.Get_mt__Etch_List());
        etypes.Add(vf.Get_mt__Etch_Map());
        etypes.Add(vf.Get_mt__Etch_Datetime());
        etypes.Add(vf.get_mt__Etch_AuthException());
        etypes.Add(vf.get_mt__exception());
		etypes.Add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazTable );
		etypes.Add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazData );
		etypes.Add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz1);
		etypes.Add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz1 );
		etypes.Add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz2 );
		etypes.Add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz2 );
		etypes.Add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz3 );
		etypes.Add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz3 );
        
       
       
		
		
        Assert.AreEqual(etypes, atypes);
		foreach (XType type in etypes )
			testGetType( vf, type );
	}
	
	private void testGetType( ValueFactory vf, XType type )
	{
	
        Assert.AreSame(type, vf.GetType(type.Id));
	
        Assert.AreSame(type,vf.GetType(type.Name));
	}

	
	[Test]
	public void Method_testValueFactoryBarGetTypes() 
	{
        DefaultValueFactory vf = new ValueFactoryBar("none:");
		List<XType> atypes = (List<XType>)vf.GetTypes();
		
		List<XType> etypes = new List<XType>();


        etypes.Add(vf.get_mt__Etch_RuntimeException());
        etypes.Add(vf.Get_mt__Etch_List());
        etypes.Add(vf.Get_mt__Etch_Map());
        etypes.Add(vf.Get_mt__Etch_Datetime());
        etypes.Add(vf.get_mt__Etch_AuthException());
        etypes.Add(vf.get_mt__exception());
        
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar_BarTable);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar_BarData);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar1);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar1);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar2);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar2);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar3);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar3);

        
       
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazTable);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazData);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz1);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz1);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz2);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz2);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz3);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz3);
		
		Assert.AreEqual( etypes, atypes );
		foreach (XType type in etypes )
			testGetType( vf, type );
	}
	
	
	[Test]
	public void Method_testValueFactoryFooGetTypes() 
	{
        DefaultValueFactory vf = new ValueFactoryFoo("none:");
		List<XType> atypes = (List<XType>)vf.GetTypes();
		
		List<XType> etypes = new List<XType>();

        etypes.Add(vf.get_mt__Etch_RuntimeException());
        etypes.Add(vf.Get_mt__Etch_List());
        etypes.Add(vf.Get_mt__Etch_Map());
        etypes.Add(vf.Get_mt__Etch_Datetime());
        etypes.Add(vf.get_mt__Etch_AuthException());
        etypes.Add(vf.get_mt__exception());
            
        etypes.Add(ValueFactoryFoo._mt_org_apache_etch_tests_Foo_FooTable);
        etypes.Add(ValueFactoryFoo._mt_org_apache_etch_tests_Foo_FooData);
        etypes.Add(ValueFactoryFoo._mt_org_apache_etch_tests_Foo_foo1);
        etypes.Add(ValueFactoryFoo._mt_org_apache_etch_tests_Foo__result_foo1);
        etypes.Add(ValueFactoryFoo._mt_org_apache_etch_tests_Foo_foo2);
        etypes.Add(ValueFactoryFoo._mt_org_apache_etch_tests_Foo__result_foo2);
        etypes.Add(ValueFactoryFoo._mt_org_apache_etch_tests_Foo_foo3);
        etypes.Add(ValueFactoryFoo._mt_org_apache_etch_tests_Foo__result_foo3);


      
       
       
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar_BarTable);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar_BarData);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar1);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar1);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar2);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar2);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar3);
        etypes.Add(ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar3);


       
       
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazTable);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazData);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz1);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz1);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz2);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz2);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz3);
        etypes.Add(ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz3);
		
	 
		
		Assert.AreEqual( etypes, atypes );
		foreach (XType type in etypes )
			testGetType( vf, type );
	}
	
	
	[Test]
	public void Method_duplicatesAreTheSame() 
	{
        Assert.AreEqual(new ValueFactoryBaz("none:").GetTypes(), new ValueFactoryBaz("none:").GetTypes());
        Assert.AreEqual(new ValueFactoryBar("none:").GetTypes(), new ValueFactoryBar("none:").GetTypes());
        Assert.AreEqual(new ValueFactoryFoo("none:").GetTypes(), new ValueFactoryFoo("none:").GetTypes());
        
	} 

    }
}
