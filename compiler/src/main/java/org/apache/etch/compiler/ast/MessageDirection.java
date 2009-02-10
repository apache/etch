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
 * The message direction is whether the message is from client to
 * server or server to client or both.
 */
public enum MessageDirection
{
	/**
	 * This message is used by the client to invoke the server.
	 */
	SERVER,
	/**
	 * This message is used by the server to invoke the client.
	 */
	CLIENT,
	/**
	 * This message may be used by either the client or server to invoke the other.
	 */
	BOTH
}

