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

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.bindings.java.transport.TransportMessage;

/**
 * Adapter between remote and message source.
 */
public interface DeliveryService extends SessionMessage, TransportMessage
{
	/**
	 * Sends the message which begins a call sequence.
	 * 
	 * @param msg the message to send.
	 * @return a mailbox which can be used to read the response, using
	 * {@link #endcall(Mailbox, Type)}.
	 * @throws Exception if there is a problem sending
	 */
	public Mailbox begincall( Message msg ) throws Exception;

	/**
	 * Finishes a call sequence by waiting for the response message.
	 * 
	 * @param mb a mailbox which will be used to read the response, returned by
	 * {@link #begincall(Message)}.
	 * @param responseType the type of the expected response.
	 * @return the value of the response field if it isn't an exception.
	 * @throws Exception if there is a problem sending or a timeout waiting or
	 * if the result value was an exception.
	 */
	public Object endcall( Mailbox mb, Type responseType ) throws Exception;
}
