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

import java.nio.channels.SelectableChannel;
import java.nio.channels.SelectionKey;

/**
 * A handler which wraps a SelectableChannel.
 * @param <T> the subclass of SelectableChannel for this Handler.
 */
public interface Handler<T extends SelectableChannel>
{
	/**
	 * @return the handler's channel.
	 */
	public T channel();

	/**
	 * @return the computed interest ops.
	 * @throws Exception 
	 */
	public int getInterestOps() throws Exception;

	/**
	 * @return the last computed interest ops saved per
	 * {@link #setLastInterestOps(int)}.
	 */
	public int getLastInterestOps();

	/**
	 * Saves the last computed interest ops set in the selection key.
	 * @param ops
	 */
	public void setLastInterestOps( int ops );

	/**
	 * Notifies the handler that it has been registered with the Selector.
	 * @param selector the Selector holding the registration.
	 * @param key the selection key.
	 * @throws Exception 
	 */
	public void registered( Selector selector, SelectionKey key )
		throws Exception;

	/**
	 * @return the selection key of the channel.
	 */
	public SelectionKey key();

	/**
	 * Notifies the handler that it has been selected. When selected returns,
	 * the {@link #getInterestOps()} method will be called to get any updates
	 * to the interests. If this method throws an exception, the Handler will
	 * be canceled.
	 * @throws Exception 
	 */
	public void selected() throws Exception;

	/**
	 * Notifies the handler that its registration has been canceled. Any
	 * Selector or SelectionKey information should be forgotten, and perhaps
	 * the channel should be closed.
	 * @param e an exception which caused the cancellation.
	 * @throws Exception 
	 */
	public void canceled( Exception e ) throws Exception;
	
	/**
	 * Reports an exception which is causing this handler to be canceled. The
	 * exception is by default reported to the selector if still registered,
	 * or printed on stderr if not registered. Override this method to report
	 * the problem some other way.
	 * @param e
	 */
	public void reportException( Exception e );
}