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
 * Main program for CuaeClient. This program makes a connection to the
 * listener created by MainCuaeListener.
 */
public class MainCuaeClient implements CuaeHelper.CuaeClientFactory
{
	/**
	 * Main program for CuaeClient.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String uri = "tcp://localhost:4006";
		if (args.length > 0)
			uri = args[0];

		MainCuaeClient implFactory = new MainCuaeClient();
		
		RemoteCuaeServer server = CuaeHelper.newServer( uri, null,
			implFactory );

		server._startAndWaitUp( 4000 );

		System.out.println( "--- doit1( 23 ) -------------------------------------------" );
		System.out.printf( "doit1( 23 ) => %s\n",
			server.doit1( new Cuae.Request( 23 ) ) );

		System.out.println( "--- doit1( 23, \"skiddoo\" ) -------------------------------------------" );
		System.out.printf( "doit1( 23, \"skiddoo\" ) => %s\n",
			server.doit1( new Cuae.ReqWithMessage( 23, "skiddoo" ) ) );

		System.out.println( "--- doit1( 19 ) -------------------------------------------" );
		System.out.printf( "doit1( 19 ) => %s\n",
			server.doit1( new Cuae.Request( 19 ) ) );

		System.out.println( "--- doit1( 13, \"unlucky friday\" ) -------------------------------------------" );
		System.out.printf( "doit1( 13, \"unlucky friday\" ) => %s\n",
			server.doit1( new Cuae.ReqWithMessage( 13, "unlucky friday" ) ) );
		
		System.out.println( "--- doit( [23] ) -------------------------------------------" );
		System.out.printf( "doit3( [23] ) => [%s]\n",
			server.doit3( new Cuae.Request[] { new Cuae.Request( 23 ) } )[0] );

		Thread.sleep( 60*1000 );
		
		server._stopAndWaitDown( 4000 );
	}

	public CuaeClient newCuaeClient( RemoteCuaeServer server )
		throws Exception
	{
		return new ImplCuaeClient( server );
	}
}
