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

package org.apache.etch.compiler.ast;

/**
 * Flag that controls how the message receiver dispatches
 * and runs the implementation method.
 */
public enum AsyncReceiverMode
{
	/**
	 * Async receiver mode is disabled. The message runs synchronously.
	 */
	NONE,
	
	/**
	 * Async receiver queued mode. The message is queued to a thread pool.
	 */
	QUEUED,
	
	/**
	 * Async receiver queued mode. The message is queued to a thread pool.
	 * This is an alias for QUEUED.
	 * @deprecated use queued instead.
	 */
	@Deprecated
	POOL,
	
	/**
	 * Async receiver free mode. THe message runs on a free thread.
	 */
	FREE
}
