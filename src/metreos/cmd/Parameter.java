/* $Id: Parameter.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Mar 27, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.cmd;

import java.lang.reflect.InvocationTargetException;

/**
 * A command line value which is specified by position. A parameter
 * is dependent upon position on the command line to be recognised.
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
