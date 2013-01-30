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

/**
 * A free implementation of Pool.
 */
public class FreePool implements Pool
{
	/**
	 * Constructs a FreePool with specified maxSize.
	 * @param maxSize maximum number of free threads at one
	 * time. after that, reject requests.
	 */
	public FreePool( int maxSize )
	{
		this.maxSize = maxSize;
	}
	
	/**
	 * Constructs a FreePool with maxSize 50.
	 */
	public FreePool()
	{
		this( 50 );
	}
	
	private final int maxSize;
	
	private final ThreadGroup tg = new ThreadGroup( "FreePoolThreadGroup" );
	
	private boolean open = true;
	
	/**
	 * Closes the pool. This just marks the pool as being closed, it doesn't
	 * actually do anything to the currently running thread. But no more
	 * threads are allowed to start.
	 */
	public void close()
	{
		open = false;
	}
	
	/**
	 * Joins each of the threads in this pool until there
	 * are none left. The pool will be closed first.
	 * @throws InterruptedException
	 */
	public void join() throws InterruptedException
	{
		close();
		Thread[] list = new Thread[10];
		int n;
		while ((n = tg.enumerate( list )) > 0)
			for (int i = 0; i < n; i++)
				list[i].join();
	}
	
	public synchronized void run( final PoolRunnable runnable )
	{
		if (!open || tg.activeCount() >= maxSize)
			throw new IllegalStateException( "free pool thread count exceeded or pool closed" );
		
		new Thread( tg, new Runnable()
		{
			public void run()
			{
				try
				{
					runnable.run();
				}
				catch ( Exception e )
				{
					runnable.exception( e );
				}
			}
		} ).start();
	}
}