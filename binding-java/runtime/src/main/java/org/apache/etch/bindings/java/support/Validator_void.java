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
 * Validator for void.
 */
public class Validator_void implements Validator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_void get( int nDims )
    {
		if (nDims != 0)
			throw new IllegalArgumentException( "nDims != 0" );
		
		if (validator == null)
			validator = new Validator_void();
		
		return validator;
    }
	
	private static Validator_void validator;
	
	/**
	 * Constructs the Validator.
	 */
	private Validator_void()
	{
		// nothing to do.
	}

    public Validator elementValidator()
    {
	    return null;
    }

	final public boolean validate( Object value )
	{
		return value == null;
	}

	public Object validateValue( Object value )
	{
		if (validate( value ))
			return value;
		
		throw new IllegalArgumentException( "value not appropriate for void: "+value );
	}
}
