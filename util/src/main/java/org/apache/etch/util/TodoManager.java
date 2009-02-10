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

package org.apache.etch.util;

/**
 * A standalone verion of a processor for todo items.
 */
public class TodoManager extends AbstractStartable implements Runnable
{
	/**
	 * Constructs the TodoManager.
	 * @param maxEntries the maximum number of entries in the queue before
	 * the caller of addEntry is delayed.
	 * @param entryDelay milliseconds to delay a caller who tries to
	 * add a entry over the limit.
	 * @param minWorkers the minimum number of workers to keep waiting.
	 * @param maxWorkers the maxumum number of workers to allow.
	 * @param workerLinger milliseconds a worker will wait for a Todo
	 * before considering quitting.
	 * @param threshold the per worker threshold for queue length. If
	 * queue length exceeds this amount, a new worker is added if allowed.
	 */
	public TodoManager( int maxEntries, int entryDelay, int minWorkers,
		int maxWorkers, int workerLinger, int threshold )
	{
		if (maxEntries < 1)
			throw new IllegalArgumentException( "maxEntries < 1" );
		
		if (entryDelay < 1)
			throw new IllegalArgumentException( "entryDelay < 1" );
		
		if (minWorkers < 0)
			throw new IllegalArgumentException( "minWorkers < 0" );
		
		if (maxWorkers < 1)
			throw new IllegalArgumentException( "maxWorkers < 1" );
		
		if (maxWorkers < minWorkers)
			throw new IllegalArgumentException( "maxWorkers < minWorkers" );
		
		if (workerLinger < 1)
			throw new IllegalArgumentException( "workerLinger < 1" );
		
		if (threshold < 0)
			throw new IllegalArgumentException( "threshold < 0" );
		
		this.maxEntries = maxEntries;
		this.entryDelay = entryDelay;
		this.minWorkers = minWorkers;
		this.maxWorkers = maxWorkers;
		this.workerLinger = workerLinger;
		this.threshold = threshold;
	}
	
	private final int maxEntries;
	
	private final int entryDelay;
	
	private final int minWorkers;
	
	private final int maxWorkers;
	
	private final int workerLinger;
	
	private final int threshold;

	@Override
    protected void start0() throws Exception
    {
	    // nothing to do.
    }

	@Override
    protected void stop0() throws Exception
    {
	    // nothing to do.
		notifyAll();
    }
	
	/**
	 * @param todo
	 * @throws InterruptedException
	 */
	public synchronized void add( Todo todo ) throws InterruptedException
	{
		checkIsStarted();
		
		int n = addEntry( todo );
		notify();
		
		considerStartingAWorker( n );
		
		if (n > maxEntries)
			Thread.sleep( entryDelay );
	}

	public void run()
	{
		boolean needsAdjust = true;
		try
		{
			Todo todo;
			while ((todo = getNextTodo()) != null)
			{
				try
				{
					todo.doit( this );
				}
				catch ( Exception e )
				{
					todo.exception( this, e );
				}
			}
			needsAdjust = false;
		}
		finally
		{
			if (needsAdjust)
				workers.adjust( -1 );
		}
	}

	/////////////
	// WORKERS //
	/////////////
	
	/**
	 * @return the number of workers.
	 */
	public int numWorkers()
	{
		return workers.get();
	}

	private synchronized void considerStartingAWorker( int qlen )
    {
	    int n = numWorkers();
	    
	    if (n >= maxWorkers)
	    	return;
	    
	    // start a new worker if there are none or if the queue
	    // length per worker has exceeded the threshold.
	    
	    if (n == 0 || (qlen + n - 1) / n > threshold)
	    	startAWorker();
	    
    }
	
	private void startAWorker()
	{
		workers.adjust( 1 );
		new Thread( this ).start();
	}
	
	private final IntCounter workers = new IntCounter();

	///////////
	// QUEUE //
	///////////
	
	/**
	 * Adds the todo to the tail of the queue.
	 * @param todo the todo to add.
	 * @return the current queue length.
	 */
	private synchronized int addEntry( Todo todo )
    {
		Entry e = new Entry();
		
		e.todo = todo;
		
		if (tail != null)
			tail.next = e;
		else
			head = e;
		
		tail = e;
	    
		return entries.adjust( 1 );
    }
	
	/**
	 * @return a todo from the head of the queue, or null if the
	 * queue is empty.
	 */
	private synchronized Todo removeEntry()
    {
	    if (head == null)
	    	return null;
	    
	    Entry e = head;
	    head = e.next;
	    
	    if (head == null)
	    	tail = null;
	    
	    entries.adjust( -1 );
	    
	    return e.todo;
    }
	
	/**
	 * @return the number of todos.
	 */
	public int numEntries()
	{
		return entries.get();
	}
	
	private Entry head;
	
	private Entry tail;
	
	private final IntCounter entries = new IntCounter();
	
	/** An entry in the todo queue */
	public static class Entry
	{
		/** the todo to be performed. */
		public Todo todo;
		/** the next todo in the queue. */
		public Entry next;
	}

	//////////
	// BLAH //
	//////////
	
	private synchronized Todo getNextTodo()
	{
		Todo todo = null;
		boolean lingered = false;
		while (isStarted() && (todo = removeEntry()) == null)
		{
			try
			{
				if (lingered && workers.get() > minWorkers)
				{
					workers.adjust( -1 );
					return null;
				}
				
				wait( workerLinger );
				
				// we lingered. we might have been woken because
				// we're stopping, or a todo might have been
				// queued.
				
				lingered = true;
			}
			catch ( InterruptedException e )
			{
				workers.adjust( -1 );
				return null;
			}
		}
		return todo;
	}

	//////////////////
	// STATIC STUFF //
	//////////////////
	
	/**
	 * @param todo
	 */
	public static void addTodo( Todo todo )
	{
		try
		{
			getTodoManager().add( todo );
		}
		catch ( Exception e )
		{
			todo.exception( null, e );
		}
	}

	/**
	 * @return the configured todo manager. if there isn't one, it makes
	 * one with one worker thread.
	 * @throws Exception if there is a problem creating the todo manager.
	 */
	public static TodoManager getTodoManager() throws Exception
	{
		if (todoManager == null)
		{
			synchronized (TodoManager.class)
			{
				if (todoManager == null)
				{
					todoManager = new TodoManager( 50, 10, 0, 5, 5000, 0 );
					todoManager.start();
				}
			}
		}
		return todoManager;
	}
	
	/**
	 * @param newTodoManager
	 * @return the old todo manager.
	 */
	public static synchronized TodoManager setTodoManager( TodoManager newTodoManager )
	{
		TodoManager oldTodoManager = todoManager;
		todoManager = newTodoManager;
		return oldTodoManager;
	}
	
	/**
	 * Shuts down the currently configured static todo manager if any.
	 * @throws Exception
	 */
	public static void shutdown() throws Exception
	{
		TodoManager oldTodoManager = setTodoManager( null );
		if (oldTodoManager != null)
			oldTodoManager.stop();
	}
	
	private static TodoManager todoManager;
}
