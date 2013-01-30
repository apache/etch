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

package org.apache.etch.util.cmd;

import java.util.HashSet;
import java.util.Set;


/**
 * A Constraint which limits a value to one of a few
 * choices.
 */
public class OneOfConstraint implements Constraint
{
	/**
	 * Constructs the OneOfConstraint.
	 *
	 * @param value the allowed values
	 */
	public OneOfConstraint( String ... value )
	{
		set = new HashSet<String>();
		for (String s: value)
			set.add( s );
	}

	/**
	 * Constructs the OneOfConstraint.
	 *
	 * @param set the set of allowed values.
	 */
	public OneOfConstraint( Set<String> set )
	{
		this.set = set;
	}
	
	private final Set<String> set;
	
	@Override
	public String toString()
	{
		return "one of "+set;
	}
	
	public void checkValue( Object value ) throws Exception
	{
		if (!set.contains( value ))
			throw new Exception( ""+value+" is not valid, expected one of "+set );
	}
}