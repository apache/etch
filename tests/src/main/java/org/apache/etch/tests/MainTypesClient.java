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

import java.util.ArrayList;
import java.util.Date;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

import org.apache.etch.util.Assertion;



/**
 * Main program for TypesClient. This program makes a connection to the
 * listener created by MainTypesListener.
 */
@SuppressWarnings("unchecked")
public class MainTypesClient implements TypesHelper.TypesClientFactory
{
	/**
	 * Main program for TypesClient.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String uri = "tcp://localhost:4001";
		if (args.length > 0)
			uri = args[0];

		MainTypesClient implFactory = new MainTypesClient();
		
		RemoteTypesServer server = TypesHelper.newServer( uri, null,
			implFactory );

		server._startAndWaitUp( 4000 );

		Map map1 = new HashMap();
		map1.put( "a", 12345678 );
		map1.put( "b", true );
		map1.put( "c", 15.5 );
		map1.put( "d", "xyz" );
		System.out.println( "map1 = "+map1 );
		Map map2 = server.test1( map1 );
		System.out.println( "map2 = "+map2 );
		Assertion.check( map1.equals( map2 ), "map1.equals( map2 )" );
		map1 = null;
		map2 = null;
		
		Map foo = new HashMap();
		foo.put( "abc", 123 );
		System.out.println( "foo1 = "+foo );
		System.out.println( "foo2 = "+server.test1( foo ) );
		foo = null;
		
		Map bar = new HashMap();
		bar.put( 123, "abc" );
		System.out.println( "bar1 = "+bar );
		System.out.println( "bar2 = "+server.test1( bar ) );
		bar = null;
		
		Map baz = new TreeMap();
		baz.put( 789, "ghi" );
		baz.put( 123, "abc" );
		baz.put( 456, "def" );
		System.out.println( "baz1 = "+baz );
		System.out.println( "baz2 = "+server.test1( baz ) );
		baz = null;
		
		Set set1 = new HashSet();
		set1.add( 12345678 );
		set1.add( true );
		set1.add( 15.5 );
		set1.add( "xyz" );
		Set set2 = server.test2( set1 );
		System.out.println( "set1 = "+set1 );
		System.out.println( "set2 = "+set2 );
		Assertion.check( set1.equals( set2 ), "set1.equals( set2 )" );
		set1 = null;
		set2 = null;
		
		List list1 = new ArrayList();
		list1.add( 12345678 );
		list1.add( true );
		list1.add( 15.5 );
		list1.add( "xyz" );
		List list2 = server.test3( list1 );
		System.out.println( "list1 = "+list1 );
		System.out.println( "list2 = "+list2 );
		Assertion.check( list1.equals( list2 ), "list1.equals( list2 )" );
		list1 = null;
		list2 = null;
		
		Date datetime1 = new Date();
		Date datetime2 = server.test4( datetime1 );
		System.out.println( "datetime1 = "+datetime1 );
		System.out.println( "datetime2 = "+datetime2 );
		Assertion.check( datetime1.equals( datetime2 ), "datetime1.equals( datetime2 )" );
		datetime1 = null;
		datetime2 = null;
		
		server._stopAndWaitDown( 4000 );
	}

	public TypesClient newTypesClient( RemoteTypesServer server )
		throws Exception
	{
		return new ImplTypesClient( server );
	}
}
