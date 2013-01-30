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

import java.lang.reflect.InvocationTargetException;

/**
 * A command line value which is specified by position. A parameter
 * is dependent upon position on the command line to be recognized.
 */
public abstract class Parameter extends OptParamBase
{
	/**
	 * @param cp
	 * @param name
	 * @param method
	 * @param params
	 * @param description
	 * @param isRequired
	 * @param constraint
	 * @throws Exception
	 */
	public Parameter( CommandParser cp, String name, String method,
		Class<?>[] params, String description, boolean isRequired,
		Constraint constraint ) throws Exception
	{
		super( cp, name, method, params, description, isRequired, constraint );
	}

	@Override
	public String toString()
	{
		return getNames()[0];
	}

	//////////
	// HELP //
	//////////

	@Override
	public void showShortDescription()
	{
		System.err.print( ' ' );
		System.err.print( getNames()[0] );
	}

	@Override
	public void showLongDescription()
	{
		System.err.print( NAME_FLAG );
		System.err.println( getNames()[0] );
		
		showDescription();
		
		showIsRequired();
		
		showConstraint();
	}

	///////////
	// VALUE //
	///////////

	/**
	 * @param value
	 * @return true if the value is ok, false otherwise.
	 */
	public boolean checkValue( Object value )
	{
		return super.checkValue( "parameter '"+toString()+"'",
			new Object[] { value } );
	}

	/**
	 * @param value
	 * @return value returned by called method.
	 * @throws Exception
	 */
	public boolean deliverValue( Object value ) throws Exception
	{
		try
		{
			return callMethod( getArgs( value ) );
		}
		catch ( InvocationTargetException e )
		{
			Throwable x = e.getTargetException();
			if (x instanceof Exception)
				throw (Exception) x;
			throw new RuntimeException( x );
		}
	}
	
	/**
	 * @param value
	 * @return the args to pass to the method.
	 */
	private Object[] getArgs( Object value )
	{
		return new Object[] { getCommandParser(), this, value };
	}
}
