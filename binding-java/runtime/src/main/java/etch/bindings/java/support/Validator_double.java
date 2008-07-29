/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for double.
 */
public class Validator_double extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_double get( int nDims )
	{
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_double( nDims );
		
		Validator_double v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_double( nDims );
		
		return v;
	}
	
	private static Validator_double[] validators = new Validator_double[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_double( int nDims )
	{
		super( Double.class, double.class, nDims, "double["+nDims+"]" );
	}

	public Validator elementValidator()
	{
		return get( nDims-1 );
	}
}
