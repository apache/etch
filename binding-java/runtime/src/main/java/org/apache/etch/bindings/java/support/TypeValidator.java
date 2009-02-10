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

import java.lang.reflect.Array;

import org.apache.etch.bindings.java.msg.Validator;


/**
 * Base class of validators which validate based on exact type.
 */
abstract public class TypeValidator implements Validator
{
	/**
	 * Constructs the TypeValidator.
	 *
	 * @param scalarClass class to use if nDims == 0
	 * @param arrayClass class to use if nDims > 0
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @param descr a description of the expected value.
	 */
	public TypeValidator( Class<?> scalarClass, Class<?> arrayClass, int nDims,
		String descr )
	{
		this( scalarClass, arrayClass, nDims, descr, false );
	}
	
	/**
	 * Constructs the TypeValidator.
	 *
	 * @param scalarClass class to use if nDims == 0
	 * @param arrayClass class to use if nDims > 0
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @param descr a description of the expected value.
	 * @param subclassOk true of a subclass of the expected class is ok.
	 */
	public TypeValidator( Class<?> scalarClass, Class<?> arrayClass, int nDims,
		String descr, boolean subclassOk )
	{
		checkDims( nDims );
		expectedClass = mkExpectedClass( scalarClass, arrayClass, nDims );
		this.nDims = nDims;
		this.descr = descr;
		this.subclassOk = subclassOk;
	}
	
	private final Class<?> expectedClass;
	
	/**
	 * The number of dimensions if this is an array.
	 */
	protected final int nDims;
	
	private final String descr;
	
	/**
	 * Allow subclassing of this type.
	 */
	protected final boolean subclassOk;
	
	/**
	 * Checks the number of dimensions for standard validators.
	 * @param nDims
	 * @throws IllegalArgumentException if nDims < 0 || nDims > MAX_NDIMS
	 * @see #MAX_NDIMS
	 */
	public final static void checkDims( int nDims )
		throws IllegalArgumentException
	{
		if (nDims < 0 || nDims > MAX_NDIMS)
			throw new IllegalArgumentException( "nDims < 0 || nDims > MAX_NDIMS" );
	}

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
	
	public boolean validate( Object value )
	{
		if (value == null)
			return false;
		
		Class<?> clss = value.getClass();
		
		if (clss == expectedClass)
			return true;
		
		// if clss is a subclass of expectedClass, that's ok.
		
		return subclassOk && expectedClass.isAssignableFrom( clss );
	}

	public Object validateValue( Object value )
	{
		if (validate( value ))
			return value;
		
		throw new IllegalArgumentException( "value not appropriate for "+descr+": "+value );
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
