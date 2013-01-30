/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.tests;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertSame;

import java.util.HashSet;
import java.util.Set;

import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.DefaultValueFactory;
import org.apache.etch.tests.Bar.BarData;
import org.apache.etch.tests.Baz.BazData;
import org.apache.etch.tests.Foo.FooData;
import org.junit.Test;


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
		DefaultValueFactory vf = new ValueFactoryBaz( "tcp:" );
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
		
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazTable );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazData );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz1 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz1 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz2 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz2 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz3 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz3 );
		
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
		DefaultValueFactory vf = new ValueFactoryBar( "tcp:" );
		Set<Type> atypes = vf.getTypes();
		
		Set<Type> etypes = new HashSet<Type>();
		etypes.add( vf.get_mt__Etch_RuntimeException() );
		etypes.add( vf.get_mt__Etch_List() );
		etypes.add( vf.get_mt__Etch_Map() );
		etypes.add( vf.get_mt__Etch_Set() );
		etypes.add( vf.get_mt__Etch_Datetime() );
		etypes.add( vf.get_mt__Etch_AuthException() );
		etypes.add( vf.get_mt__exception() );
		
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazTable );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazData );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz1 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz1 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz2 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz2 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz3 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz3 );
		
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar_BarTable );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar_BarData );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar1 );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar1 );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar2 );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar2 );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar3 );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar3 );
		
		assertEquals( etypes, atypes );
		for (Type type: etypes )
			testGetType( vf, type );
	}
	
	/** @throws Exception */
	@Test
	public void testValueFactoryFooGetTypes() throws Exception
	{
		DefaultValueFactory vf = new ValueFactoryFoo( "tcp:" );
		Set<Type> atypes = vf.getTypes();
		
		Set<Type> etypes = new HashSet<Type>();
		etypes.add( vf.get_mt__Etch_RuntimeException() );
		etypes.add( vf.get_mt__Etch_List() );
		etypes.add( vf.get_mt__Etch_Map() );
		etypes.add( vf.get_mt__Etch_Set() );
		etypes.add( vf.get_mt__Etch_Datetime() );
		etypes.add( vf.get_mt__Etch_AuthException() );
		etypes.add( vf.get_mt__exception() );

		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazTable );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_BazData );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz1 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz1 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz2 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz2 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz_baz3 );
		etypes.add( ValueFactoryBaz._mt_org_apache_etch_tests_Baz__result_baz3 );

		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar_BarTable );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar_BarData );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar1 );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar1 );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar2 );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar2 );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar_bar3 );
		etypes.add( ValueFactoryBar._mt_org_apache_etch_tests_Bar__result_bar3 );

		etypes.add( ValueFactoryFoo._mt_org_apache_etch_tests_Foo_FooTable );
		etypes.add( ValueFactoryFoo._mt_org_apache_etch_tests_Foo_FooData );
		etypes.add( ValueFactoryFoo._mt_org_apache_etch_tests_Foo_foo1 );
		etypes.add( ValueFactoryFoo._mt_org_apache_etch_tests_Foo__result_foo1 );
		etypes.add( ValueFactoryFoo._mt_org_apache_etch_tests_Foo_foo2 );
		etypes.add( ValueFactoryFoo._mt_org_apache_etch_tests_Foo__result_foo2 );
		etypes.add( ValueFactoryFoo._mt_org_apache_etch_tests_Foo_foo3 );
		etypes.add( ValueFactoryFoo._mt_org_apache_etch_tests_Foo__result_foo3 );
		
		assertEquals( etypes, atypes );
		for (Type type: etypes )
			testGetType( vf, type );
	}
	
	/** @throws Exception */
	@Test
	public void duplicatesAreTheSame() throws Exception
	{
		assertEquals( new ValueFactoryBaz( "tcp:" ).getTypes(), new ValueFactoryBaz( "tcp:" ).getTypes() );
		assertEquals( new ValueFactoryBar( "tcp:" ).getTypes(), new ValueFactoryBar( "tcp:" ).getTypes() );
		assertEquals( new ValueFactoryFoo( "tcp:" ).getTypes(), new ValueFactoryFoo( "tcp:" ).getTypes() );
	}
}
