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


/**
 * Implementation of methods for FooClient.
 */
public class ImplFooClient implements FooClient
{
	/**
	 * Constructs the ImplFooClient.
	 * @param server
	 */
	public ImplFooClient( RemoteFooServer server )
	{
		this.server = server;
	}
	
	@SuppressWarnings("unused")
	protected FooServer server;

	public Boolean foo1( Foo.FooData fooData )
	{
		throw new UnsupportedOperationException();
	}

	public Boolean foo3( Foo.FooData fooData )
	{
		throw new UnsupportedOperationException();
	}

	public Boolean bar1( BarData barData )
	{
		throw new UnsupportedOperationException();
	}

	public Boolean bar3( BarData barData )
	{
		throw new UnsupportedOperationException();
	}

	public Boolean baz1( BazData bazData )
	{
		throw new UnsupportedOperationException();
	}

	public Boolean baz3( BazData bazData )
	{
		throw new UnsupportedOperationException();
	}
}
