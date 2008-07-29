

using etch.bindings.csharp.examples.mixin;
using etch.bindings.csharp.examples.mixin.bar;
using etch.bindings.csharp.examples.mixin.baz;
using Etch.Support;
using NUnit.Framework;
using System;
using System.Collections.Generic;
using System.Text;
using Etch.Msg;
using etch.bindings.csharp.examples.mixin.baz.types.Baz;
using etch.bindings.csharp.examples.mixin.bar.types.Bar;
using etch.bindings.csharp.examples.mixin.types.Foo;

namespace Etch.Compiler.Test
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
		DefaultValueFactory vf = new ValueFactoryBaz();
		List<XType> atypes = (List<XType>)vf.GetTypes();
		
		List<XType> etypes = new List<XType>();


        etypes.Add(vf.get_mt__Etch_RuntimeException());
        etypes.Add(vf.Get_mt__Etch_List());
        etypes.Add(vf.Get_mt__Etch_Map());
        etypes.Add(vf.Get_mt__Etch_Datetime());
        etypes.Add(vf.get_mt__Etch_AuthException());
        etypes.Add(vf.get_mt__exception());
		etypes.Add( ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz_BazTable );
		etypes.Add( ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz_BazData );
		etypes.Add( ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz_baz1);
		etypes.Add( ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz__result_baz1 );
		etypes.Add( ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazServer_baz2 );
		etypes.Add( ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazClient__result_baz2 );
		etypes.Add( ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazClient_baz3 );
		etypes.Add( ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazServer__result_baz3 );
        
       
       
		
		
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
		DefaultValueFactory vf = new ValueFactoryBar();
		List<XType> atypes = (List<XType>)vf.GetTypes();
		
		List<XType> etypes = new List<XType>();


        etypes.Add(vf.get_mt__Etch_RuntimeException());
        etypes.Add(vf.Get_mt__Etch_List());
        etypes.Add(vf.Get_mt__Etch_Map());
        etypes.Add(vf.Get_mt__Etch_Datetime());
        etypes.Add(vf.get_mt__Etch_AuthException());
        etypes.Add(vf.get_mt__exception());
        
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_Bar_BarTable);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_Bar_BarData);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_Bar_bar1);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_Bar__result_bar1);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_BarServer_bar2);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_BarClient__result_bar2);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_BarClient_bar3);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_BarServer__result_bar3);

        
       
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz_BazTable);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz_BazData);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz_baz1);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz__result_baz1);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazServer_baz2);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazClient__result_baz2);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazClient_baz3);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazServer__result_baz3);
		
		Assert.AreEqual( etypes, atypes );
		foreach (XType type in etypes )
			testGetType( vf, type );
	}
	
	
	[Test]
	public void Method_testValueFactoryFooGetTypes() 
	{
		DefaultValueFactory vf = new ValueFactoryFoo();
		List<XType> atypes = (List<XType>)vf.GetTypes();
		
		List<XType> etypes = new List<XType>();

        etypes.Add(vf.get_mt__Etch_RuntimeException());
        etypes.Add(vf.Get_mt__Etch_List());
        etypes.Add(vf.Get_mt__Etch_Map());
        etypes.Add(vf.Get_mt__Etch_Datetime());
        etypes.Add(vf.get_mt__Etch_AuthException());
        etypes.Add(vf.get_mt__exception());
            
        etypes.Add(ValueFactoryFoo._mt_etch_bindings_csharp_examples_mixin_Foo_FooTable);
        etypes.Add(ValueFactoryFoo._mt_etch_bindings_csharp_examples_mixin_Foo_FooData);
        etypes.Add(ValueFactoryFoo._mt_etch_bindings_csharp_examples_mixin_Foo_foo1);
        etypes.Add(ValueFactoryFoo._mt_etch_bindings_csharp_examples_mixin_Foo__result_foo1);
        etypes.Add(ValueFactoryFoo._mt_etch_bindings_csharp_examples_mixin_FooServer_foo2);
        etypes.Add(ValueFactoryFoo._mt_etch_bindings_csharp_examples_mixin_FooClient__result_foo2);
        etypes.Add(ValueFactoryFoo._mt_etch_bindings_csharp_examples_mixin_FooClient_foo3);
        etypes.Add(ValueFactoryFoo._mt_etch_bindings_csharp_examples_mixin_FooServer__result_foo3);


      
       
       
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_Bar_BarTable);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_Bar_BarData);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_Bar_bar1);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_Bar__result_bar1);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_BarServer_bar2);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_BarClient__result_bar2);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_BarClient_bar3);
        etypes.Add(ValueFactoryBar._mt_etch_bindings_csharp_examples_mixin_bar_BarServer__result_bar3);


       
       
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz_BazTable);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz_BazData);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz_baz1);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_Baz__result_baz1);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazServer_baz2);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazClient__result_baz2);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazClient_baz3);
        etypes.Add(ValueFactoryBaz._mt_etch_bindings_csharp_examples_mixin_baz_BazServer__result_baz3);
		
	 
		
		Assert.AreEqual( etypes, atypes );
		foreach (XType type in etypes )
			testGetType( vf, type );
	}
	
	
	[Test]
	public void Method_duplicatesAreTheSame() 
	{
        Assert.AreEqual(new ValueFactoryBaz().GetTypes(), new ValueFactoryBaz().GetTypes());
        Assert.AreEqual(new ValueFactoryBar().GetTypes(), new ValueFactoryBar().GetTypes());
        Assert.AreEqual(new ValueFactoryFoo().GetTypes(), new ValueFactoryFoo().GetTypes());
        
	} 

    }
}
