package etch.examples.mixin.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertSame;

import java.util.HashSet;
import java.util.Set;

import org.junit.Test;

import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.DefaultValueFactory;
import etch.examples.mixin.Foo;
import etch.examples.mixin.FooClient;
import etch.examples.mixin.FooServer;
import etch.examples.mixin.ValueFactoryFoo;
import etch.examples.mixin.Foo.FooData;
import etch.examples.mixin.bar.Bar;
import etch.examples.mixin.bar.BarClient;
import etch.examples.mixin.bar.BarServer;
import etch.examples.mixin.bar.ValueFactoryBar;
import etch.examples.mixin.bar.Bar.BarData;
import etch.examples.mixin.baz.Baz;
import etch.examples.mixin.baz.BazClient;
import etch.examples.mixin.baz.BazServer;
import etch.examples.mixin.baz.ValueFactoryBaz;
import etch.examples.mixin.baz.Baz.BazData;

/** Test of mixin */
@SuppressWarnings("unused")
public class TestMixin
{
	/** @throws Exception */
	@Test
	public void testBaz() throws Exception
	{
		BazData bazData = null;
		
		Baz baz = null;
		if (baz != null)
		{
			baz.baz1( bazData );
		}
	}
	
	/** @throws Exception */
	@Test
	public void testBazClient() throws Exception
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
	
	/** @throws Exception */
	@Test
	public void testBazServer() throws Exception
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
	
	/** @throws Exception */
	@Test
	public void testBar() throws Exception
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
	
	/** @throws Exception */
	@Test
	public void testBarClient() throws Exception
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
	
	/** @throws Exception */
	@Test
	public void testBarServer() throws Exception
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
	
	/** @throws Exception */
	@Test
	public void testFoo() throws Exception
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
	
	/** @throws Exception */
	@Test
	public void testFooClient() throws Exception
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
	
	/** @throws Exception */
	@Test
	public void testFooServer() throws Exception
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
	
	/** @throws Exception */
	@Test
	public void testValueFactoryBazGetTypes() throws Exception
	{
		DefaultValueFactory vf = new ValueFactoryBaz();
		Set<Type> atypes = vf.getTypes();
		
		Set<Type> etypes = new HashSet<Type>();
		etypes.add( vf.get_mt__Etch_RuntimeException() );
		etypes.add( vf.get_mt__Etch_List() );
		etypes.add( vf.get_mt__Etch_Map() );
		etypes.add( vf.get_mt__Etch_Set() );
		etypes.add( vf.get_mt__Etch_Datetime() );
		etypes.add( vf.get_mt__Etch_AuthException() );
		etypes.add( vf.get_mt__Etch_AuthException() );
		etypes.add( vf.get_mt__exception() );
		
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz_BazTable );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz_BazData );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz_baz1 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz__result_baz1 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazServer_baz2 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazClient__result_baz2 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazClient_baz3 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazServer__result_baz3 );
		
		assertEquals( etypes, atypes );
		for (Type type: etypes )
			testGetType( vf, type );
	}
	
	private void testGetType( ValueFactory vf, Type type )
	{
		assertSame( type, vf.getType( type.getId() ) );
		assertSame( type, vf.getType( type.getName() ) );
	}

	/** @throws Exception */
	@Test
	public void testValueFactoryBarGetTypes() throws Exception
	{
		DefaultValueFactory vf = new ValueFactoryBar();
		Set<Type> atypes = vf.getTypes();
		
		Set<Type> etypes = new HashSet<Type>();
		etypes.add( vf.get_mt__Etch_RuntimeException() );
		etypes.add( vf.get_mt__Etch_List() );
		etypes.add( vf.get_mt__Etch_Map() );
		etypes.add( vf.get_mt__Etch_Set() );
		etypes.add( vf.get_mt__Etch_Datetime() );
		etypes.add( vf.get_mt__Etch_AuthException() );
		etypes.add( vf.get_mt__exception() );
		
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz_BazTable );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz_BazData );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz_baz1 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz__result_baz1 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazServer_baz2 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazClient__result_baz2 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazClient_baz3 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazServer__result_baz3 );
		
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_Bar_BarTable );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_Bar_BarData );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_Bar_bar1 );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_Bar__result_bar1 );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_BarServer_bar2 );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_BarClient__result_bar2 );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_BarClient_bar3 );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_BarServer__result_bar3 );
		
		assertEquals( etypes, atypes );
		for (Type type: etypes )
			testGetType( vf, type );
	}
	
	/** @throws Exception */
	@Test
	public void testValueFactoryFooGetTypes() throws Exception
	{
		DefaultValueFactory vf = new ValueFactoryFoo();
		Set<Type> atypes = vf.getTypes();
		
		Set<Type> etypes = new HashSet<Type>();
		etypes.add( vf.get_mt__Etch_RuntimeException() );
		etypes.add( vf.get_mt__Etch_List() );
		etypes.add( vf.get_mt__Etch_Map() );
		etypes.add( vf.get_mt__Etch_Set() );
		etypes.add( vf.get_mt__Etch_Datetime() );
		etypes.add( vf.get_mt__Etch_AuthException() );
		etypes.add( vf.get_mt__exception() );

		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz_BazTable );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz_BazData );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz_baz1 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_Baz__result_baz1 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazServer_baz2 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazClient__result_baz2 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazClient_baz3 );
		etypes.add( ValueFactoryBaz._mt_etch_examples_mixin_baz_BazServer__result_baz3 );

		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_Bar_BarTable );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_Bar_BarData );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_Bar_bar1 );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_Bar__result_bar1 );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_BarServer_bar2 );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_BarClient__result_bar2 );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_BarClient_bar3 );
		etypes.add( ValueFactoryBar._mt_etch_examples_mixin_bar_BarServer__result_bar3 );

		etypes.add( ValueFactoryFoo._mt_etch_examples_mixin_Foo_FooTable );
		etypes.add( ValueFactoryFoo._mt_etch_examples_mixin_Foo_FooData );
		etypes.add( ValueFactoryFoo._mt_etch_examples_mixin_Foo_foo1 );
		etypes.add( ValueFactoryFoo._mt_etch_examples_mixin_Foo__result_foo1 );
		etypes.add( ValueFactoryFoo._mt_etch_examples_mixin_FooServer_foo2 );
		etypes.add( ValueFactoryFoo._mt_etch_examples_mixin_FooClient__result_foo2 );
		etypes.add( ValueFactoryFoo._mt_etch_examples_mixin_FooClient_foo3 );
		etypes.add( ValueFactoryFoo._mt_etch_examples_mixin_FooServer__result_foo3 );
		
		assertEquals( etypes, atypes );
		for (Type type: etypes )
			testGetType( vf, type );
	}
	
	/** @throws Exception */
	@Test
	public void duplicatesAreTheSame() throws Exception
	{
		assertEquals( new ValueFactoryBaz().getTypes(), new ValueFactoryBaz().getTypes() );
		assertEquals( new ValueFactoryBar().getTypes(), new ValueFactoryBar().getTypes() );
		assertEquals( new ValueFactoryFoo().getTypes(), new ValueFactoryFoo().getTypes() );
	}
}
