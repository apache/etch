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

package org.apache.etch.util.core.io;

import java.net.InetAddress;

import org.apache.etch.util.core.Who;



/**
 * Who for an inet address / port.
 */
public class InetWho implements Who
{
	/**
	 * @param addr
	 * @param port
	 */
	public InetWho( InetAddress addr, int port )
	{
		this.addr = addr;
		this.port = port;
	}
	
	private final InetAddress addr;
	
	private final int port;

	/**
	 * @return the address of who.
	 */
	public InetAddress getInetAddress()
	{
		return addr;
	}

	/**
	 * @return the port of who.
	 */
	public int getPort()
	{
		return port;
	}

	/**
	 * @param addr
	 * @param port
	 * @return true if the specified addr and port match this who.
	 */
	public boolean matches( InetAddress addr, int port )
	{
		return this.addr.equals( addr ) && this.port == port;
	}
}