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
 * A circular queue of a fixed size. Elements are added to one
 * end and removed from the other, with the queue size ranging
 * from empty to full. Operations can optionally wait until
 * finished or return status indicating success or failure.
 * For instance, adding to a full queue can wait until an item
 * is removed before adding the new item or it can wait only
 * a specified amount of time before completing successfully
 * or giving up.
 * @param <T> the type of items in the queue.
 */
public final class CircularQueue<T>
{
	/**
	 * Constructs the CircularQueue.
	 *
	 * @param size the maximum number of items allowed in the queue.
	 */
	public CircularQueue( int size )
	{
		if (size < 1)
			throw new IllegalArgumentException( "size < 1" );
		
		this.size = size;
		items = new Object[size];
	}
	
	/**
	 * Constructs the CircularQueue with the maximum number of items
	 * defaulted to 10.
	 */
	public CircularQueue()
	{
		this( 10 );
	}

	private final int size;
	
	private final Object[] items;
	
	/**
	 * @return the maximum number of items that may be put in the queue.
	 */
	public int size()
	{
		return size;
	}

	/////////////////////
	// BASIC QUEUE OPS //
	/////////////////////
	
	/**
	 * @return the current number of items in the queue.
	 */
	public int count()
	{
		return count;
	}
	
	/**
	 * @return true if the queue is empty.
	 */
	public boolean isEmpty()
	{
		return count == 0;
	}
	
	/**
	 * @return true if the queue is full.
	 */
	public boolean isFull()
	{
		return count == size;
	}
	
	/**
	 * Gets the item at the head of the queue.
	 * @return the item at the head of the queue.
	 */
	private Object get0()
	{
		Assertion.check( count > 0, "count > 0" );
		
		Object obj = items[head];
		
		Assertion.check( obj != null, "obj != null" );
		
		items[head] = null;
		
		count--;
		head++;
		
		if (head == size)
			head = 0;
		
		return obj;
	}
	
	/**
	 * Puts the item at the tail of the queue.
	 * @param obj non-null item to put.
	 */
	private void put0( Object obj )
	{
		Assertion.check( obj != null, "obj != null" );
		Assertion.check( count < size, "count < size" );
		Assertion.check( items[tail] == null, "items[tail] == null" );
		
		items[tail] = obj;
		
		count++;
		tail++;
		
		if (tail == size)
			tail = 0;
	}
	
	private int count;
	
	private int head;
	
	private int tail;

	//////////////////////
	// PUBLIC QUEUE OPS //
	//////////////////////
	
	/**
	 * Gets the next available item from the queue, waiting
	 * until an item is available or the queue is closed.
	 * 
	 * @return an item from the queue or null if the queue
	 * is closed.
	 * 
	 * @throws InterruptedException if the thread is interrupted.
	 */
	public T get() throws InterruptedException
	{
		return get( 0 );
	}
	
	/**
	 * Gets the next available item from the queue, waiting
	 * until an item is available or the queue is closed.
	 * 
	 * @param maxDelay the maximum time in ms to wait for
	 * something to be put in the queue; 0 means wait forever,
	 * less than 0 means don't wait at all.
	 * 
	 * @return an item from the queue or null if maxDelay
	 * has been exceeded or the queue is closed.
	 * 
	 * @throws InterruptedException if the thread is interrupted.
	 */
	public synchronized T get( int maxDelay ) throws InterruptedException
	{
		if (!isEmpty())
			return getAndNotify();
		
		// the queue is empty.
		
		if (isClosed() || maxDelay < 0)
			return null;
		
		// the queue is empty, not closed, and caller has requested a delay...
		
		long now = Timer.getNanos();
		long end = getEndTime( now, maxDelay );
		Assertion.check( end > now, "end > now" );
		
		long d;
		while ((d = (end - now) / Timer.NANOS_PER_MILLI) > 0)
		{
			// the queue is empty, not closed, and delay has not run out...
			
			Assertion.check( isEmpty(), "isEmpty()" );
			Assertion.check( !isClosed(), "!isClosed()" );
			Assertion.check( d > 0, "d > 0" );
			
			wait( d );
			
			if (!isEmpty())
				return getAndNotify();
			
			if (isClosed())
				return null;
			
			now = Timer.getNanos();
		}
		
		return null;
	}

	/**
	 * Puts an item in the queue, waiting until space is available
	 * or the queue is closed.
	 * 
	 * @param obj a non-null item to put in the queue.
	 * 
	 * @return true if the item was placed in the queue
	 * or false if the queue is closed.
	 * 
	 * @throws InterruptedException if the thread is interrupted.
	 */
	public boolean put( T obj ) throws InterruptedException
	{
		return put( obj, 0 );
	}
	
	/**
	 * Puts an item in the queue, waiting until space is available
	 * or the queue is closed.
	 * 
	 * @param obj a non-null item to put in the queue.
	 * 
	 * @param maxDelay the maximum time in ms to wait for
	 * available space the queue; 0 means wait forever,
	 * less than 0 means don't wait at all.
	 * 
	 * @return true if the item was placed in the queue
	 * or false if maxDelay has been exceeded or the queue
	 * is closed.
	 * 
	 * @throws InterruptedException if the thread is interrupted.
	 */
	public synchronized boolean put( T obj, int maxDelay ) throws InterruptedException
	{
		if (obj == null)
			throw new NullPointerException( "obj == null" );
		
		if (isClosed())
			return false;
		
		// the queue is not closed.
		
		if (!isFull())
		{
			putAndNotify( obj );
			return true;
		}
		
		// the queue is full and not closed.
		
		if (maxDelay < 0)
			return false;
		
		// the queue is full, not closed, and the caller has requested a delay...
		
		long now = Timer.getNanos();
		long end = getEndTime( now, maxDelay );
		Assertion.check( end > now, "end > now" );
		
		long d;
		while ((d = (end - now) / Timer.NANOS_PER_MILLI) > 0)
		{
			// the queue is full, not closed, and delay has not run out...
			
			Assertion.check( isFull(), "isFull()" );
			Assertion.check( !isClosed(), "!isClosed()" );
			Assertion.check( d > 0, "d > 0" );
			
			wait( d );
			
			if (isClosed())
				return false;
			
			// the queue is not closed.
			
			if (!isFull())
			{
				putAndNotify( obj );
				return true;
			}
			
			now = Timer.getNanos();
		}
		
		return false;
	}

	/**
	 * Closes the queue so that no more items may be put into it.
	 * Get will return null when there are no more items to return.
	 */
	public synchronized void close()
	{
		if (!closed)
		{
			closed = true;
			notifyAll();
		}
	}
	
	/**
	 * @return true if the queue is closed.
	 */
	public boolean isClosed()
	{
		return closed;
	}
	
	private boolean closed;
	
	//////////////////
	// PRIVATE OPS //
	//////////////////
	
	/**
	 * Gets the item at the head of the queue. Additionally, wakes
	 * up the next one waiting for the queue, either to get or put.
	 * @return the item at the head of the queue.
	 */
	@SuppressWarnings("unchecked")
	private T getAndNotify()
	{
		// the queue is now not empty.
		
		// putters waiting should be notified if the queue is now full.
		boolean notify = isFull();
		
		Object obj = get0();
		
		// getters waiting should be notified if the queue is now not empty.
		notify = notify || !isEmpty();
		
		if (notify)
			notify();
		
		return (T) obj;
	}
	
	/**
	 * Puts the item at the tail of the queue. Additionally, wakes
	 * up the next one waiting for the queue, either to get or put.
	 * @param obj non-null item to put.
	 */
	private void putAndNotify( T obj )
	{
		// the queue is now not full.
		
		// getters waiting should be notified if the queue is now empty.
		boolean notify = isEmpty();
		
		put0( obj );
		
		// putters waiting should be notified if the queue is now not full.
		notify = notify || !isFull();
		
		if (notify)
			notify();
	}

	private long getEndTime( long now, int maxDelay )
	{
		return maxDelay > 0 ? now + maxDelay * Timer.NANOS_PER_MILLI : Long.MAX_VALUE;
	}
}