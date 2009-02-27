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

/**
 * Main program for ExampleFooClient. This program makes a connection to the
 * listener created by MainExampleFooListener.
 */
public class MainExampleFooClient implements ExampleFooHelper.ExampleFooClientFactory
{
	/**
	 * Main program for ExampleFooClient.
	 * 
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		// TODO Change to correct URI
		String uri = "tcp://localhost:4002";
		
		RemoteExampleFooServer server = ExampleFooHelper.newServer( uri, null,
			new MainExampleFooClient() );

		// Connect to the service
		server._startAndWaitUp( 4000 );

		System.out.println( "helloServer = "+server.helloServer( "helloServer blah", null ) );
		System.out.println( "foo = "+server.foo( "foo blah" ) );

		// Disconnect from the service
		server._stopAndWaitDown( 4000 );
	}

	public ExampleFooClient newExampleFooClient( RemoteExampleFooServer server )
		throws Exception
	{
		return new ImplExampleFooClient( server );
	}
}
