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
 * Main program for InheritanceClient. This program makes a connection to the
 * listener created by MainInheritanceListener.
 */
public class MainInheritanceClient implements InheritanceHelper.InheritanceClientFactory
{
	/**
	 * Main program for InheritanceClient.
	 * 
	 * @param args
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		String uri = "tcp://localhost:4001";
		if (args.length > 0)
			uri = args[0];

		MainInheritanceClient implFactory = new MainInheritanceClient();
		
		RemoteInheritanceServer server = InheritanceHelper.newServer( uri, null,
			implFactory );

		server._startAndWaitUp( 4000 );

		// -------------------------------------------------------------
		// Insert Your Code Here
		// -------------------------------------------------------------

		server._stopAndWaitDown( 4000 );
	}

	public InheritanceClient newInheritanceClient( RemoteInheritanceServer server )
		throws Exception
	{
		return new ImplInheritanceClient( server );
	}
}
