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

/**
 * Description of OrConstraint.
 */
public class OrConstraint implements Constraint
{
	/**
	 * Constructs the OrConstraint.
	 * @param a
	 * @param b
	 */
	public OrConstraint( Constraint a, Constraint b )
	{
		this.a = a;
		this.b = b;
	}
	
	private final Constraint a;
	
	private final Constraint b;
	
	public void checkValue( Object value ) throws Exception
	{
		try
		{
			a.checkValue( value );
		}
		catch ( Exception ea )
		{
			try
			{
				b.checkValue( value );
			}
			catch ( Exception eb )
			{
				throw new Exception( "failed both constraints: "+
					ea.getMessage()+" AND "+eb.getMessage() );
			}
		}
	}
	
	@Override
	public String toString()
	{
		return "("+a+") OR ("+b+")";
	}
}
