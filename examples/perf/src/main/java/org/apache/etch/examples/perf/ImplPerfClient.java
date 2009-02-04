/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.examples.perf;


/**
 * Your custom implementation of BasePerfClient. Add methods here to provide
 * implementations of messages from the server.
 */
public class ImplPerfClient extends BasePerfClient
{
	/**
	 * Constructs the ImplPerfClient.
	 *
	 * @param server a connection to the server session. Use this to send a
	 * message to the server.
	 */
	public ImplPerfClient( RemotePerfServer server )
	{
		this.server = server;
	}
	
	@SuppressWarnings("unused")
	private final RemotePerfServer server;

	// TODO insert methods here to provide implementations of PerfClient
	// messages from the server.
}