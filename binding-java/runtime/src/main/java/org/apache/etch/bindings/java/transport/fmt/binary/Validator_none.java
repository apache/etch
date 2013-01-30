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

package org.apache.etch.bindings.java.transport.fmt.binary;

import org.apache.etch.bindings.java.msg.Validator;

/**
 * A validator for the special end of list marker,
 * {@link BinaryTaggedData#NONE}.
 */
public class Validator_none implements Validator
{
	/**
	 * @return the validator for BinaryTaggedData.NONE.
	 */
	public static Validator_none get()
    {
		if (validator == null)
			validator = new Validator_none();
	    
		return validator;
    }
	
	private static Validator_none validator;
	
	/**
	 * Constructs the validator.
	 */
	public Validator_none()
    {
	    // nothing to do.
    }

    public Validator elementValidator()
    {
		throw new IllegalArgumentException( "not an array" );
    }
	
	final public boolean validate( Object value )
	{
	    return value == BinaryTaggedData.NONE;
	}

	public Object validateValue( Object value )
	{
		if (validate( value ))
			return value;
		
		throw new IllegalArgumentException( "value not appropriate for none: "+value );
	}
}
