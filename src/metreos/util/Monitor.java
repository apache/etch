/* $Id: Monitor.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Feb 14, 2005
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package metreos.util;

/**
 * A class which we can use to monitor conditions.
 * 
 * @param <T> The type of object that may be stored in the monitor.
 */
public class Monitor<T>
{
	/**
	 * @param description
	 * @param initialValue
	 */
	public Monitor( String description, T initialValue )
	{
		this.description = description;
		this.value = initialValue;
	}
	
	/**
	 * @param description
	 */
	public Monitor( String description )
	{
		this( description, null );
	}
	
	/**
	 * @return the description of this monitor.
	 */
	
	public String getDescription()
	{
		return description;
	}
	
	private final String description;
	
	private T value;

	@Override
	public String toString()
	{
		return "Monitor "+description+": "+value;
	}
	
	/**
	 * @return the current value of the monitor.
	 */
	public T get()
	{
		return value;
	}
	
	/**
	 * Sets the current value.
	 * @param newValue
	 * @return the old value.
	 */
	public synchronized T set( T newValue )
	{
		T oldValue = value;
		value = newValue;
		notifyAll();
		return oldValue;
	}

	/**
	 * Waits until the monitor value is set.
	 * 
	 * @param maxDelay the max amount of time in ms to wait.
	 * If 0 is specified, we will wait forever.
	 * 
	 * @return the current value of the monitor.
	 * 
	 * @throws InterruptedException if we waited too long.
	 */
	private synchronized T waitUntilSet( long maxDelay )
		throws InterruptedException
		
	{
		wait( maxDelay );
		return value;
	}

	/**
	 * @param desiredValue
	 * @param newValue
	 * @return the old value
	 * @throws InterruptedException
	 */
	public T waitUntilEqAndSet( T desiredValue,
		T newValue ) throws InterruptedException
	{
		return waitUntilEqAndSet( desiredValue, 0, newValue );
	}

	/**
	 * @param desiredValue
	 * @param maxDelay
	 * @param newValue
	 * @return the old value
	 * @throws InterruptedException
	 */
	public synchronized T waitUntilEqAndSet( T desiredValue,
		long maxDelay, T newValue ) throws InterruptedException
	{
		waitUntilEq( desiredValue, maxDelay );
		return set( newValue );
	}

	/**
	 * Waits forever until the value is set to the specified value.
	 * 
	 * @param desiredValue the value we are waiting for.
	 * 
	 * @throws InterruptedException
	 */
	public void waitUntilEq( T desiredValue ) throws InterruptedException
	{
		waitUntilEq( desiredValue, 0 );
	}


	/**
	 * Waits until the value is set to the specified value.
	 * 
	 * @param desiredValue the value we are waiting for.
	 * 
	 * @param maxDelay the max amount of time in ms to wait.
	 * If 0 is specified, we will wait forever.
	 * 
	 * @throws InterruptedException if we waited too long.
	 */
	public synchronized void waitUntilEq( T desiredValue, long maxDelay )
		throws InterruptedException
	{
		long now = Timer.currentTimeMillis();
		long endTime = maxDelay > 0 ?
			now + maxDelay : Long.MAX_VALUE;
		
		while (!eq( value, desiredValue ) && now < endTime)
		{
			waitUntilSet( endTime - now );
			now = Timer.currentTimeMillis();
		}
		
		if (!eq( value, desiredValue ))
			throw new InterruptedException( "timeout" );
	}

	/**
	 * @param undesiredValue
	 * @param newValue
	 * @return the old value
	 * @throws InterruptedException
	 */
	public T waitUntilNotEqAndSet( T undesiredValue, T newValue )
		throws InterruptedException
	{
		return waitUntilNotEqAndSet( undesiredValue, 0, newValue );
	}

	/**
	 * @param undesiredValue
	 * @param maxDelay
	 * @param newValue
	 * @return the old value
	 * @throws InterruptedException
	 */
	public synchronized T waitUntilNotEqAndSet( T undesiredValue,
		long maxDelay, T newValue ) throws InterruptedException
	{
		waitUntilNotEq( undesiredValue );
		return set( newValue );
	}
	
	/**
	 * Waits forever until the value is not the specified value.
	 * 
	 * @param undesiredValue the value we do not want.
	 * 
	 * @return the current value of the monitor.
	 * 
	 * @throws InterruptedException if we waited too long.
	 */
	public T waitUntilNotEq( T undesiredValue )
		throws InterruptedException
	{
		return waitUntilNotEq( undesiredValue, 0 );
	}
	
	/**
	 * Waits until the value is not the specified value.
	 * 
	 * @param undesiredValue the value we do not want.
	 * 
	 * @param maxDelay the max amount of time in ms to wait.
	 * If 0 is specified, we will wait forever.
	 * 
	 * @return the current value of the monitor.
	 * 
	 * @throws InterruptedException if we waited too long.
	 */
	public synchronized T waitUntilNotEq( T undesiredValue, long maxDelay )
		throws InterruptedException
	{
		long now = Timer.currentTimeMillis();
		long endTime = maxDelay > 0 ?
			now + maxDelay : Long.MAX_VALUE;
		
		while (eq( value, undesiredValue ) && now < endTime)
		{
			waitUntilSet( endTime - now );
			now = Timer.currentTimeMillis();
		}
		
		if (eq( value, undesiredValue ))
			throw new InterruptedException( "timeout" );
		
		return value;
	}

	/**
	 * Compares the specified values.
     * 
	 * @param v1 a value to compare, which may be null.
	 * 
	 * @param v2 another value to compare, which may be null.
	 * 
	 * @return true if the values are equal, false otherwise. If both
	 * values are null, they are considered equal.
	 */
	private boolean eq( T v1, T v2 )
	{
		if (v1 != null && v2 != null)
			return v1.equals( v2 );
		
		return v1 == v2;
	}
}
















