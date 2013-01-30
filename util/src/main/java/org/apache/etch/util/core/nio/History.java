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

/**
 * History keeps track of pooled resource allocations, plus maintains historical
 * information to better judge what a reasonable pool size for resources is.
 */
public class History
{
	/**
	 * Constructs a History.
	 * @param min the minimum number of used items.
	 * @param limit the maximum number of used items.
	 * @param len the length of the history of used items over time.
	 */
	public History( int min, int limit, int len )
	{
		if (min < 0)
			throw new IllegalArgumentException( "min < 0" );
		
		if (limit < min)
			throw new IllegalArgumentException( "limit < min" );

		if (len < 0)
			throw new IllegalArgumentException( "len < 0" );

		this.min = min;
		this.limit = limit;
		history = new int[len];
		
		init();
	}

	/** The minimum number of used items. */
	private final int min;

	/** The maximum number of used items. */
	private final int limit;

	/** The history of used items over time. */
	private final int[] history;

	/**
	 * @return the maximum number of used items.
	 */
	public int limit()
	{
		return limit;
	}

	/**
	 * @return the minimum number of used items.
	 */
	public int min()
	{
		return min;
	}
	
	/**
	 * @return the length of the history.
	 */
	public int length()
	{
		return history.length;
	}

	/**
	 * Adjusts the number of items being used. This is the number of items which
	 * have been allocated but not yet released.
	 * 
	 * @param k positive if items are being allocated, negative if items are
	 * being released. Items might be released by being returned to some system
	 * facility, or they might be saved in a pool.
	 * @return true if the new value of used does not exceed the limit.
	 * @throws IllegalStateException if the new value of used would be < 0.
	 */
	public boolean used( int k )
	{
		if (k == 0)
			throw new IllegalArgumentException( "k == 0" );

		int newUsed = used + k;

		if (newUsed > limit)
			return false; // overflow

		if (newUsed < 0)
			throw new IllegalStateException( "newUsed < 0" );

		used = newUsed;
		bump( used );
		return true;
	}

	/**
	 * @return the number of items being used.
	 */
	public int getUsed()
	{
		return used;
	}

	private int used;

	/**
	 * Adjusts the number of items actually allocated or freed from some system
	 * facility rather than just from a pool. The number of used items
	 * can never exceed the number allocated. The number of allocated items
	 * equals used plus the current size of the pool.
	 * 
	 * @param k positive if items are being allocated, negative if items are
	 * being released.
	 */
	public void alloc( int k )
	{
		if (k == 0)
			throw new IllegalArgumentException( "k == 0" );

		int newAlloc = alloc + k;

		if (newAlloc > limit)
			throw new IllegalStateException( "newAlloc > limit" );

		if (newAlloc < 0)
			throw new IllegalStateException( "newAlloc < 0" );

		alloc = newAlloc;
	}

	/**
	 * @return the number of items allocated.
	 */
	public int getAlloc()
	{
		return alloc;
	}

	private int alloc;

	/**
	 * Initialize (or re-initialize) the history.
	 */
	public void init()
	{
		synchronized (history)
		{
			int h = Math.max( min, used );
			
			for (int i = 0; i < history.length; i++)
				history[i] = h;

			highwatermark = h;
			suggested = h;
		}
	}

	/**
	 * Tickles the history. The oldest value of history is discarded, and the
	 * others are all pushed down one, and the newest value is initialized to
	 * min. This should be called periodically on a regular schedule.
	 */
	public void tickle()
	{
		synchronized (history)
		{
			if (history.length == 0)
				return;
			
			System.arraycopy( history, 0, history, 1, history.length - 1 );
			history[0] = highwatermark;
			highwatermark = Math.max( min, used );

			// recalculate suggested size

			suggested = recalc( history );
		}
	}

	/**
	 * Recalculates the suggested size for pool of allocated but unused items.
	 * Various algorithms might be used, including those that might use average
	 * history (the default) or something based on 1st or 2nd order derivitives
	 * of that.
	 * 
	 * @return recalculated suggested size for pool of allocated but unused
	 * items.
	 */
	protected int recalc( int[] hstry )
	{
		int sum = 0;
		for (int h : hstry)
			sum += h;
		return sum / hstry.length;
	}

	/**
	 * Updates the high water mark in light of a new value of used.
	 * 
	 * @param value the new value of used.
	 */
	private void bump( int value )
	{
		synchronized (history)
		{
			if (value > highwatermark)
				highwatermark = value;
		}
	}

	private int highwatermark;

	private int suggested;

	/**
	 * @return the suggested size of a pool for quick allocation. This
	 * might be statically configured or based on recent activity.
	 */
	public int suggested()
	{
		return suggested;
	}
}
