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

import java.lang.reflect.Method;

/**
 * Shared base class of options and parameters.
 */
abstract public class OptParamBase
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
	public OptParamBase( CommandParser cp, String name, String method,
		Class<?>[] params, String description, boolean isRequired,
		Constraint constraint ) throws Exception
	{
		this.cp = cp;
		names = new String[] { name };
		this.method = findMethod( cp.program, method, params );
		this.description = description;
		this.isRequired = isRequired;
		if (constraint != null)
			constraints = new Constraint[] { constraint };
		else
			constraints = null;
	}
	
	/**
	 * @param cp
	 * @param names
	 * @param method
	 * @param params
	 * @param description
	 * @param isRequired
	 * @param constraints
	 * @throws Exception
	 */
	public OptParamBase( CommandParser cp, String[] names, String method,
		Class<?>[] params, String description, boolean isRequired,
		Constraint[] constraints ) throws Exception
	{
		if (constraints != null && constraints.length != names.length)
			throw new IllegalArgumentException(
				"constraints.length != names.length" );
		
		this.cp = cp;
		this.names = names;
		this.method = findMethod( cp.program, method, params );
		this.description = description;
		this.isRequired = isRequired;
		this.constraints = constraints;
	}
	
	private final CommandParser cp;
	
	private final String[] names;

	private final Method method;
	
	private final String description;
	
	private final boolean isRequired;
	
	private final Constraint[] constraints;
	
	/**
	 * @return the command parser of this item.
	 */
	public CommandParser getCommandParser()
	{
		return cp;
	}
	
	/**
	 * @return the name of this item.
	 */
	public String[] getNames()
	{
		return names;
	}
	
	/**
	 * @return the description of this item.
	 */
	public String getDescription()
	{
		return description;
	}
	
	/**
	 * @return true if this item is required.
	 */
	public boolean isRequired()
	{
		return isRequired;
	}
	
	//////////
	// HELP //
	//////////
	
	/**
	 * Shows a very short synopsis of the item for the command line usage message.
	 *
	 */
	abstract public void showShortDescription();
	
	/**
	 * Shows the longer "help" description which appears underneath the
	 * command line usage message.
	 *
	 */
	abstract public void showLongDescription();

	/**
	 * Shows the text description of the item.
	 */
	protected void showDescription()
	{
		System.err.print( DESC_FLAG );
		System.err.println( description );
	}

	/**
	 * Shows whether the item is required or not.
	 */
	protected void showIsRequired()
	{
		if (isRequired())
		{
			System.err.print( DESC_FLAG );
			System.err.println( "required (must be specified)." );
		}
	}
	
	/**
	 * Shows the constraint if any.
	 */
	protected void showConstraint()
	{
		if (constraints != null)
		{
			int n = constraints.length;
			System.err.print( DESC_FLAG );
			
			if (n > 1)
				System.err.print( "constraint: {" );
			else
				System.err.print( "constraint: " );
			
			for (Constraint constraint: constraints)
			{
				System.err.print( ' ' );
				System.err.print( constraint );
			}
			
			if (n > 1)
				System.err.println( " }" );
			else
				System.err.println();
		}
	}
	
	/**
	 * Flag value to use at the beginning of the line for the
	 * name of the item.
	 */
	protected final static String NAME_FLAG = "   ";
	
	/**
	 * Flag value to use at beginning of the line for descriptive
	 * text of an item.
	 */
	protected final static String DESC_FLAG = "      # ";
	
	///////////
	// VALUE //
	///////////
	
	/**
	 * Invokes the constraint checkValue method if there is
	 * a constraint.
	 * 
	 * @param who
	 * 
	 * @param values
	 * 
	 * @return true if the parameter is ok, false otherwise. If false
	 * is returned, a message has been printed on System.err.
	 */
	public boolean checkValue( String who, Object[] values )
	{
		if (constraints != null)
		{
			int n = constraints.length;
			for (int i = 0; i < n; i++)
			{
				Constraint constraint = constraints[i];
				
				if (constraint == null)
					continue;
				
				try
				{
					constraint.checkValue( values[i] );
				}
				catch ( Exception e )
				{
					System.err.print( who );
					System.err.print( ": validation of value " );
					System.err.print( names[i] );
					System.err.print( "='" );
					System.err.print( values[i] );
					System.err.print( "' failed: " );
					System.err.println( e.getMessage() );
					return false;
				}
			}
		}
		return true;
	}
	
	/**
	 * @param value
	 * @return value converted as appropriate for this item
	 */
	abstract protected Object convertValue( String value );

	////////////
	// METHOD //
	////////////

	/**
	 * @param program
	 * @param method
	 * @param params
	 * @return the method for prog given params.
	 * @throws Exception
	 */
	private static Method findMethod( Program program, String method,
		Class<?>[] params ) throws Exception
	{
		return program.getClass().getMethod( method, params );
	}

	/**
	 * @param args
	 * @return value returned from method, or true if null returned.
	 * @throws Exception
	 */
	public boolean callMethod( Object[] args )
		throws Exception
	{
		Object result = method.invoke( cp.program, args );
		
		if (result == null)
			return true;
		
		return ((Boolean) result).booleanValue();
	}
}
