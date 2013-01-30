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

import java.util.List;
import java.util.Map;

/**
 * Interface to a test. A test consists of setup, support, jig, and cleanup
 * programs.
 */
public interface TestIntf
{
	/**
	 * @return the containing InteropTestIntf.
	 */
	public InteropTestIntf itest();
	
	/**
	 * @return the name of the test.
	 */
	public String name();
	
	/**
	 * @return the params of this test.
	 */
	public Map<String, Param> params();
	
	/**
	 * @return the setup programs of this test.
	 */
	public List<Prog> setup();
	
	/**
	 * @return the support programs of this test.
	 */
	public List<Prog> support();
	
	/**
	 * @return the test jig of this test. This is the program that performs the
	 * test.
	 */
	public Prog jig();
	
	/**
	 * @return the cleanup programs of this test.
	 */
	public List<Prog> cleanup();

	/**
	 * Runs the test with the specified arguments.
	 * @param overrides name / value pairs which must correspond to defined
	 * test parameters and override their values.
	 * @throws Exception 
	 */
	public void run( Map<String, String> overrides ) throws Exception;

	/**
	 * @param name
	 * @return the named Program
	 */
	public Program getProgram( String name );
}
