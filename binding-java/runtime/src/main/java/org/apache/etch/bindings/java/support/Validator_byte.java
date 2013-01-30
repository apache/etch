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

import org.apache.etch.bindings.java.msg.Validator;

/**
 * Validator for byte.
 */
public class Validator_byte extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_byte get( int nDims )
	{
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_byte( nDims );
		
		Validator_byte v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_byte( nDims );
		
		return v;
	}
	
	private static Validator_byte[] validators = new Validator_byte[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_byte( int nDims )
	{
		super( Byte.class, byte.class, nDims, "byte["+nDims+"]" );
	}

	public Validator elementValidator()
	{
		return get( nDims-1 );
	}
	
	@Override
	public boolean validate( Object value )
	{
		if (nDims > 0)
			return super.validate( value );
		
		if (value == null)
			return false;
		
		Class<?> c = value.getClass();
		
		if (c == Byte.class)
			return true;
		
		if (c == Short.class)
		{
			short v = (Short) value;
			return v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE;
		}
		
		if (c == Integer.class)
		{
			int v = (Integer) value;
			return v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE;
		}
		
		if (c == Long.class)
		{
			long v = (Long) value;
			return v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE;
		}
		
		return false;
	}
	
	@Override
	public Object validateValue( Object value )
	{
		value = super.validateValue( value );
		
		if (value.getClass() == Byte.class || nDims > 0)
			return value;
		
		return ((Number) value).byteValue();
	}
}
