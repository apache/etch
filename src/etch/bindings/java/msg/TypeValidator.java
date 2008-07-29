/* $Id$
 *
 * Created by wert on Aug 27, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import java.lang.reflect.Array;

/**
 * Base class of validators which validate based on exact type.
 */
abstract public class TypeValidator extends Validator
{
	/**
	 * Constructs the TypeValidator.
	 *
	 * @param scalarClass class to use if nDims == 0
	 * @param arrayClass class to use if nDims > 0
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @param descr
	 */
	public TypeValidator( Class<?> scalarClass, Class<?> arrayClass, int nDims,
		String descr )
	{
		if (nDims < 0)
			throw new IllegalArgumentException( "nDims < 0" );
		
		this.expectedClass = mkExpectedClass( scalarClass, arrayClass, nDims );
		this.nDims = nDims;
		this.descr = descr;
	}
	
	private final Class<?> expectedClass;
	
	/**
	 * The number of dimensions if this is an array.
	 */
	protected final int nDims;
	
	private final String descr;

	/**
	 * @return the expected class of this validator.
	 */
	public Class<?> getExpectedClass()
	{
		return expectedClass;
	}
	
	/**
	 * @return the expected number of dimensions.
	 */
	public int getNDims()
	{
		return nDims;
	}
	
	@Override
	final public String toString()
	{
		return descr;
	}
	
	@Override
	final public boolean validate( Object value )
	{
		return value.getClass() == expectedClass;
	}
	
	/**
	 * @param scalarClass class to use if nDims == 0
	 * @param arrayClass class to use if nDims > 0
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an appropriate class given nDims.
	 */
	private static Class<?> mkExpectedClass( Class<?> scalarClass,
		Class<?> arrayClass, int nDims )
	{
		if (nDims == 0)
			return scalarClass;
		
		return Array.newInstance( arrayClass, new int[nDims] ).getClass();
	}
}
