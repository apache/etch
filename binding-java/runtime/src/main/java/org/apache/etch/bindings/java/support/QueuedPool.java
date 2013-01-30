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

import org.apache.etch.util.Todo;
import org.apache.etch.util.TodoManager;

/**
 * A queued implementation of pool.
 */
public class QueuedPool implements Pool
{
	/**
	 * Constructs the queued pool with a specified todo manager.
	 * @param mgr
	 */
	public QueuedPool( TodoManager mgr )
	{
		this.mgr = mgr;
	}
	
	/**
	 * Constructs the queued pool with a default todo manager.
	 * @throws Exception
	 */
	public QueuedPool() throws Exception
	{
		this( TodoManager.getTodoManager() );
	}
	
	private final TodoManager mgr;
	
	public void run( final PoolRunnable runnable ) throws Exception
    {
        mgr.add( new Todo()
        {
			public void doit( TodoManager mgr ) throws Exception
            {
                runnable.run();
            }

			public void exception( TodoManager mgr, Exception e )
            {
                runnable.exception( e );
            }
        } );
    }
}