/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.tests;

import org.apache.etch.tests.BaseFooServer;
import org.apache.etch.tests.Foo;
import org.apache.etch.tests.RemoteFooClient;

/**
 * Implementation of methods for FooServer.
 */
public class ImplFooServer extends BaseFooServer
{
	/**
	 * Constructs the ImplFooServer.
	 *
	 * @param client the connection to our client.
	 */
	public ImplFooServer( RemoteFooClient client )
	{
		this.client = client;
	}
	
	@SuppressWarnings("unused")
	private final RemoteFooClient client;

	public Boolean foo1( Foo.FooData fooData )
	{
		System.out.println( "foo1 called with "+fooData );
		return true;
	}

	public Boolean foo2( Foo.FooData fooData )
	{
		System.out.println( "foo2 called with "+fooData );
		return false;
	}

	public Boolean bar1( BarData barData )
	{
		System.out.println( "bar1 called with "+barData );
		return true;
	}

	public Boolean bar2( BarData barData )
	{
		System.out.println( "bar2 called with "+barData );
		return false;
	}

	public Boolean baz1( BazData bazData )
	{
		System.out.println( "baz1 called with "+bazData );
		return true;
	}

	public Boolean baz2( BazData bazData )
	{
		System.out.println( "baz2 called with "+bazData );
		return false;
	}

	public void _sessionNotify( Object event ) throws Exception
	{
		System.out.println( event );
	}
}
