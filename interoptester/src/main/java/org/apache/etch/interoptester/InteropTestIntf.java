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

package org.apache.etch.interoptester;

import java.util.Map;

/**
 * Interface to a suite of tests to be run.
 */
public interface InteropTestIntf
{
	/**
	 * @param test the name of the test to get.
	 * @return the interface to the test.
	 */
	public TestIntf getTest( String test );

	/**
	 * @param name
	 * @return the named Program.
	 */
	public Program getProgram( String name );
	
	/**
	 * Runs all the tests in the suite.
	 * 
	 * @param overrides replacement values for interoptest params. any params
	 * which have no default must be specified.
	 * @throws Exception 
	 */
	public void run( Map<String, String> overrides ) throws Exception;

	/**
	 * @return the next id of a run in this interoptest.
	 */
	public int nextRunId();
}
