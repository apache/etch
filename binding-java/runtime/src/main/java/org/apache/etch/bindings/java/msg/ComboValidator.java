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

package org.apache.etch.bindings.java.msg;

/**
 * Validates values using one of two validators.
 */
public class ComboValidator implements Validator
{
	/**
	 * Constructs the ComboValidator.
	 *
	 * @param a
	 * @param b
	 */
	public ComboValidator( Validator a, Validator b )
	{
		this.a = a;
		this.b = b;
	}
	
	private final Validator a;
	
	private final Validator b;
	
	@Override
	public String toString()
	{
		return String.format( "(%s OR %s)", a, b );
	}

	public Validator elementValidator()
	{
		Validator na;
		try
		{
			na = a.elementValidator();
		}
		catch ( IllegalArgumentException e )
		{
			na = null;
		}
		
		Validator nb;
		try
		{
			nb = b.elementValidator();
		}
		catch ( IllegalArgumentException e )
		{
			nb = null;
		}
		
		if (na == null && nb == null)
			throw new IllegalArgumentException( "na == null && nb == null" );
		
		if (na == null)
			return nb;
		
		if (nb == null)
			return na;
		
		return new ComboValidator( na, nb );
	}

	public boolean validate( Object value )
	{
		return a.validate( value ) || b.validate( value );
	}

	public Object validateValue( Object value )
	{
		try
		{
			return a.validateValue( value );
		}
		catch (IllegalArgumentException e )
		{
			return b.validateValue( value );
		}
	}

	/**
	 * @return the "a" child of this validator.
	 */
	public Validator a()
	{
		return a;
	}

	/**
	 * @return the "b" child of this validator.
	 */
	public Validator b()
	{
		return b;
	}
}
