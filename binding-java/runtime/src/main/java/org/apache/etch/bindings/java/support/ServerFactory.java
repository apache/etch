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

package org.apache.etch.bindings.java.support;

import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.util.core.io.Session;


/**
 * Interface to use for constructing new server instances by
 * TransportHelper.
 */
public interface ServerFactory extends Session
{
	/**
	 * @param m the TransportMessage to use with the new server.
	 * @param vf the value factory to use with the new server.
	 * @return the constructed DeliveryService
	 * @throws Exception
	 */
	public DeliveryService newServer( TransportMessage m, ValueFactory vf ) throws Exception;
	
	/**
	 * @return a new instance of value factory for this connection.
	 */
	public ValueFactory newValueFactory();
}
