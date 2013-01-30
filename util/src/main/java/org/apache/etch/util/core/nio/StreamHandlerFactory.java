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

package org.apache.etch.util.core.nio;

import java.io.IOException;
import java.nio.channels.SocketChannel;

/**
 * Factory to create StreamHandler for AcceptHandler.
 */
public interface StreamHandlerFactory
{
	/**
	 * Creates a new StreamHandler for a SocketChannel.
	 * @param channel a channel accepted by AcceptHandler.
	 * @param wantsConnect true if this socket is connecting and
	 * needs a connected event to be selected.
	 * @return a StreamHandler for the specified channel.
	 * @throws IOException if a StreamHandler cannot be made.
	 * @throws Exception 
	 */
	public StreamHandler newStreamHandler( SocketChannel channel,
		boolean wantsConnect ) throws Exception;
}
