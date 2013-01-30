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

package org.apache.etch.util;

/**
 * A Todo is used to perform a lightweight action.
 * 
 * @see TodoManager
 */
public interface Todo
{
	/**
	 * Performs the action.
	 * @param mgr the todo manager where this todo was queued.
	 * @throws Exception if there is a problem.
	 */
	public void doit( TodoManager mgr ) throws Exception;

	/**
	 * Reports an exception that occurred while running the todo.
	 * @param mgr the todo manager where this todo was queued.
	 * @param e the exception that the todo threw.
	 */
	public void exception( TodoManager mgr, Exception e );
}
