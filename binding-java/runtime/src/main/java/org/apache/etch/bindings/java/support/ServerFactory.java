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

package org.apache.etch.bindings.java.support;

import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.util.Resources;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.Transport;


/**
 * Interface to use for constructing new server instances by
 * TransportHelper.
 */
public interface ServerFactory extends Session, Transport<Session>
{
	/**
	 * @param transport the TransportMessage to use with the new server instance.
	 * @param uri the uri to use to configure the new server instance.
	 * @param resources the resources to use for the new server instance.
	 * @throws Exception
	 */
	public void newServer( TransportMessage transport, String uri,
		Resources resources ) throws Exception;
	
	/**
	 * @param uri the uri to use to configure the new value factory.
	 * @return a new instance of value factory for this connection.
	 */
	public ValueFactory newValueFactory( String uri );
}
