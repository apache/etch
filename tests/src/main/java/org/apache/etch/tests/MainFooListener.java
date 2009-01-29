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

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.tests.FooHelper;
import org.apache.etch.tests.FooServer;
import org.apache.etch.tests.RemoteFooClient;
import org.apache.etch.tests.FooHelper.FooServerFactory;
import org.apache.etch.util.core.io.Transport;


/**
 * Main implementation for FooListener.
 */
public class MainFooListener implements FooServerFactory
{
	/**
 	 * Main for FooListener.
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args)
		throws Exception
	{
		String uri = "tcp://0.0.0.0:4001";

		Transport<ServerFactory> listener = FooHelper.newListener( uri, null, new MainFooListener() );

		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}

	/**
	 * Constructs the MainFooListener.
	 */
	public MainFooListener()
	{
		// Nothing to do.
	}

	/**
	 * Return a new instance of FooServer.
	 * @param client
	 */
	public FooServer newFooServer( RemoteFooClient client )
	{
		return new ImplFooServer( client );
	}
}
