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

import org.junit.Before;
import org.junit.Test;

/**
 * Simple tests to ensure that includes defined in TestIncludes.etch 
 * work properly and all included variables & messages can be accessed
 *
 * @author dattas
 */



public class TestImplIncludes
{
	
	private RemoteTestIncludesClient tc = null;
	private RemoteTestIncludesServer ts = null;
	private ImplTestIncludesServer testServer = null;
	private ImplTestIncludesClient testClient = null;
	
	/** */
	@Before public void setup() {
		testServer = new ImplTestIncludesServer(tc);
		testClient = new ImplTestIncludesClient(ts);
	}
	
	/** */
	@Test public void testConsts() {
		
		try
		{	
			testServer.testConst2();
			testServer.testConst3();
			Integer x = new Integer(5);
			Integer y = new Integer(50);
			testClient.testConst1( x, y );
			testClient.testConst2();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
		}		
	}
	
	/** */
	@Test public void testEnums() {
		try
		{
			testServer.testEnum2();
			testClient.testEnum1();
		}
		catch ( UnsupportedOperationException e )
		{
			// ignore
		}	
	}
	
}
