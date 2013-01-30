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

import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.Validator;

/**
 * Validator for StructValue.
 */
public class Validator_StructValue extends TypeValidator
{
	/**
	 * @param type the expected type of a scalar StructValue.
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_StructValue get( Type type, int nDims )
	{
		checkDims( nDims );
		
//		if (nDims >= validators.length)
			return new Validator_StructValue( type, nDims );
		
//		Validator_StructValue v = validators[nDims];
//
//		if (v == null)
//			v = validators[nDims] = new Validator_StructValue( type, nDims );
//
//		return v;
	}
	
//	private static Validator_StructValue[] validators = new Validator_StructValue[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_StructValue( Type type, int nDims )
	{
		super( StructValue.class, StructValue.class, nDims, "StructValue["+type+", "+nDims+"]" );
		this.type = type;
	}
	
	private final Type type;

	/**
	 * @return the type of this validator.
	 */
	public Type getType()
	{
		return type;
	}

	public Validator elementValidator()
	{
		return get( type, nDims-1 );
	}

	@Override
	public boolean validate( Object value )
	{
		return super.validate( value ) &&
			(value.getClass() != StructValue.class ||
				type.isAssignableFrom( ((StructValue) value).type() ) );
	}
}
