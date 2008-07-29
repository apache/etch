/* $Id: IdGenerator.java 21902 2006-04-12 13:28:38Z wert $
 *
 * Created by wert on Feb 27, 2006.
 *
 * Copyright (c) 2007 Metreos, Inc. All rights reserved.
 */

package metreos.util;

/**
 * Synchronously generates long id numbers.
 */
final public class IdGenerator
{
	/**
	 * Constructs the IdGenerator with the default starting point of 1
	 * and the default stride of 1.
	 */
	public IdGenerator()
	{
		this( 1, 1 );
	}
	
	/**
	 * Constructs the IdGenerator with the specified starting point
	 * and the default stride of 1.
	 *
	 * @param nextId
	 */
	public IdGenerator( long nextId )
	{
		this( nextId, 1 );
	}
	
	/**
	 * Constructs the IdGenerator with the specified starting point
	 * and the specified stride;
	 * @param nextId
	 * @param stride
	 */
	public IdGenerator( long nextId, int stride )
	{
		if (stride <= 0)
			throw new IllegalArgumentException( "stride <= 0" );
		
		this.nextId = nextId;
		this.stride = stride;
	}
	
	private long nextId;
	
	private final int stride;
	
	/**
	 * @return the next id in sequence.
	 */
	public synchronized long next()
	{
		long id = nextId;
		nextId += stride;
		return id;
	}
}
