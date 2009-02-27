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

package org.apache.etch.examples.example;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.examples.example.ExampleHelper.ExampleServerFactory;
import org.apache.etch.examples.example.FooHelper.FooServerFactory;
import org.apache.etch.util.Resources;
import org.apache.etch.util.core.io.Transport;

/**
 * Main program for FooServer. This program makes a listener to accept
 * connections from MainFooClient.
 */
public class MainFooListener implements FooServerFactory, ExampleServerFactory
{
	/**
	 * Main program for FooServer.
	 * 
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		// TODO Change to correct URI
		String uri = "tcp://0.0.0.0:4002";
		
		MainFooListener factory = new MainFooListener();
		
		Resources res = new Resources();
		res.put( ExampleServerFactory.class.getName(), (ExampleServerFactory) factory );
		
		ServerFactory listener = FooHelper.newListener( uri, res, factory );

		// Start the Listener
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}

	public ExampleServer newExampleServer( RemoteExampleClient client )
		throws Exception
	{
		return new ImplExampleServer( client );
	}

	public FooServer newFooServer( RemoteFooClient client )
	{
		return new ImplFooServer( client );
	}
}
