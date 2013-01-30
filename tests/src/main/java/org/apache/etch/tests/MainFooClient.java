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

import org.apache.etch.bindings.java.util.StrStrHashMap;
import org.apache.etch.tests.Bar.BarData;
import org.apache.etch.tests.Baz.BazData;
import org.apache.etch.tests.Foo.FooData;
import org.apache.etch.tests.FooHelper.FooClientFactory;


/**
 * Main implementation for FooClient.
 */
public class MainFooClient implements FooClientFactory
{
	/**
	 * Main for FooClient
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args)
		throws Exception
	{
		String uri = "tcp://localhost:4001";

		RemoteFooServer server = FooHelper.newServer( uri, null, new MainFooClient() );
		
		server._startAndWaitUp( 4000 );
		
		StrStrHashMap bazTable = new StrStrHashMap();
		bazTable.put( "def", "baz" );
		
		StrStrHashMap barTable = new StrStrHashMap();
		barTable.put( "def", "bar" );
		
		StrStrHashMap fooTable = new StrStrHashMap();
		fooTable.put( "def", "foo" );
		
		BazData bazData = new BazData( 1, bazTable );
		BarData barData = new BarData( 2, barTable, bazData );
		FooData fooData = new FooData( 3, fooTable, barData, bazData );

		boolean ok;
		
		ok = server.baz1( bazData );
		System.out.println( "baz1 => "+ok );
		
		ok = server.baz2( bazData );
		System.out.println( "baz2 => "+ok );

		ok = server.bar1( barData );
		System.out.println( "bar1 => "+ok );
		
		ok = server.bar2( barData );
		System.out.println( "bar2 => "+ok );
		
		ok = server.foo1( fooData );
		System.out.println( "foo1 => "+ok );
		
		ok = server.foo2( fooData );
		System.out.println( "foo2 => "+ok );
		
		server._stopAndWaitDown( 4000 );
	}

	public FooClient newFooClient( RemoteFooServer server ) throws Exception
	{
		return new ImplFooClient( server );
	}
}
