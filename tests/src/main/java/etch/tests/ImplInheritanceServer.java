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

package etch.tests;

/**
 * Your custom implementation of BaseInheritanceServer. Add methods here to provide
 * implementations of messages from the client.
 */
public class ImplInheritanceServer extends BaseInheritanceServer
{
	/**
	 * Constructs the ImplInheritanceServer.
	 *
	 * @param client a connection to the client session. Use this to send a
	 * message to the client.
	 */
	public ImplInheritanceServer( RemoteInheritanceClient client )
	{
		this.client = client;
	}
	
	@SuppressWarnings("unused")
	private final RemoteInheritanceClient client;

	public Object f1( Object v )
	{
		return v;
	}

	public S1 f2( S1 v )
	{
		return v;
	}

	public S2 f3( S2 v )
	{
		return v;
	}

	public S3 f4( S3 v )
	{
		return v;
	}

	public Object[] f5( Object[] v )
	{
		return v;
	}

	public S1[] f6( S1[] v )
	{
		return v;
	}

	public S2[] f7( S2[] v )
	{
		return v;
	}

	public S3[] f8( S3[] v )
	{
		return v;
	}
}