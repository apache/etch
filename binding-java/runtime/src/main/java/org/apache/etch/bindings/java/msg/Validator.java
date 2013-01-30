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
 * Validate values put into messages.
 */
public interface Validator
{
	/**
	 * The maximum number of dimensions for arrays (9).
	 */
	public final static int MAX_NDIMS = 9;
	
	/**
	 * The maximum number of validators to cache per type.
	 */
	public final static int MAX_CACHED = 4;
	
	/** Validation level for put into StructValue and TaggedDataOutput */
	public enum Level
	{
		/** No validation on put or output. */
		NONE,
		/** Validation on put or output if validator defined. */
		MISSING_OK,
		/** Like MISSING_OK, but fail if no validator defined. */
		FULL
	}
	
	/**
	 * @return for an array type, return a validator for an element of
	 * the array (which might be a scalar or another array element).
	 */
	public Validator elementValidator();

	/**
	 * Checks the value for being valid. If the value is valid
	 * by this validator, return true.
	 * @param value the value to be validated.
	 * @return true if the value is valid by any validator in the
	 * chain, or false if all reject it.
	 */
	public boolean validate( Object value );

	/**
	 * @param value
	 * @return the appropriate value given the input value and this
	 * validator's sensibility about what it should be really (e.g.,
	 * input Byte but output Long.
	 */
	public Object validateValue( Object value );
}
