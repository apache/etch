/* $Id$
 *
 * Created by sccomer on Nov 9, 2006.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package metreos.util;

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
	@SuppressWarnings("unchecked")
	public CircularQueue( int size )
	{
		this.size = size;
		items = (T[]) new Object[size];
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
	
	private final T[] items;
	
	/**
	 * @return the maximum number of items that may be put in the queue.
	 */
	public int size()
	{
		return size;
	}
	
	private int count;
	
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
	
	private int head;
	
	private T intGet()
	{
		assert !isEmpty();
		
		assert items[head] != null;
		T t = items[head];
		items[head] = null;
		
		// wake up someone waiting for space.
		if (isFull())
			notify();
		
		count--;
		
		head++;
		if (head == size)
			head = 0;
		
		return t;
	}
	
	private int tail;
	
	private void intPut( T t )
	{
		assert t != null;
		assert !isFull();
		
		assert items[tail] == null;
		items[tail] = t;
		
		// wake up someone waiting for an item.
		if (isEmpty())
			notify();
		
		count++;
		
		tail++;
		if (tail == size)
			tail = 0;
	}
	
	/**
	 * Gets the next available item from the queue, waiting
	 * until an item is available or the queue is closed.
	 * 
	 * @return the item from the queue, or null if the queue
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
	 * less than 0 means don't wait.
	 * 
	 * @return the item from the queue, or null if maxDelay
	 * has been exceeded or the queue is closed.
	 * 
	 * @throws InterruptedException if the thread is interrupted.
	 */
	public synchronized T get( int maxDelay ) throws InterruptedException
	{
		if (!isEmpty())
			return intGet();
		
		if (isClosed() || maxDelay < 0)
			return null;
		
		// the queue is empty, not closed, and caller has requested a delay
		
		assert maxDelay >= 0;
		
		long now = Timer.getNanos();
		long end = maxDelay != 0 ? now + maxDelay * Timer.NANOS_PER_MILLI : Long.MAX_VALUE;
		assert now < end;
		
		while (now < end)
		{
			// the queue is empty, not closed, and delay has not run out...
			assert isEmpty();
			assert !isClosed();

			wait( (end - now) / Timer.NANOS_PER_MILLI );
			
			if (!isEmpty())
				return intGet();
			
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
	 * @param t a non-null item to put in the queue.
	 * 
	 * @return true if the item was placed in the queue,
	 * or false if the queue is closed.
	 * 
	 * @throws InterruptedException if the thread is interrupted.
	 */
	public boolean put( T t ) throws InterruptedException
	{
		return put( t, 0 );
	}
	
	/**
	 * Puts an item in the queue, waiting until space is available
	 * or the queue is closed.
	 * 
	 * @param t a non-null item to put in the queue.
	 * 
	 * @param maxDelay the maximum time in ms to wait for
	 * available space the queue; 0 means wait forever,
	 * less than 0 means don't wait.
	 * 
	 * @return true if the item was placed in the queue,
	 * or false if maxDelay has been exceeded or the queue is closed.
	 * 
	 * @throws InterruptedException if the thread is interrupted.
	 */
	public synchronized boolean put( T t, int maxDelay ) throws InterruptedException
	{
		if (t == null)
			throw new NullPointerException( "t == null" );
		
		if (isClosed())
			return false;
		
		// the queue is not closed.
		
		if (!isFull())
		{
			intPut( t );
			return true;
		}
		
		// the queue is not closed, the queue is full.
		
		if (maxDelay < 0)
			return false;
		
		// the queue is not closed, the queue is full, and the caller has requested a delay...
		
		long now = Timer.getNanos();
		long end = maxDelay != 0 ? now + maxDelay * Timer.NANOS_PER_MILLI : Long.MAX_VALUE;
		assert now < end;
		
		while (now < end)
		{
			// the queue is not closed, the queue is full, and delay has not run out...
			assert !isClosed();
			assert isFull();
			
			wait( (end - now) / Timer.NANOS_PER_MILLI );
			
			if (isClosed())
				return false;
			
			// the queue is not closed.
			
			if (!isFull())
			{
				intPut( t );
				return true;
			}
			
			// the queue is not closed, the queue is full, and delay might have run out...
			
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
		closed = true;
		notifyAll();
	}
	
	/**
	 * @return true if the queue is closed.
	 */
	public boolean isClosed()
	{
		return closed;
	}
	
	private boolean closed;
}