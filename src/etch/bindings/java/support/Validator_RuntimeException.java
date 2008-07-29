/* $Id$
 *
 * Created by wert on Aug 20, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for RuntimeException.
 */
public class Validator_RuntimeException extends Validator
{
	/**
	 * @return an instance of the validator.
	 */
	public static Validator_RuntimeException get()
	{
		if (validator == null)
			validator = new Validator_RuntimeException();
		
		return validator;
	}
	
	private static Validator_RuntimeException validator;
	
	/**
	 * Constructs the Validator.
	 */
	private Validator_RuntimeException()
	{
		// nothing to do.
	}

	@Override
	public Validator elementValidator()
	{
		throw new IllegalArgumentException( "not an array" );
	}
	
	@Override
	public boolean validate( Object value )
	{
		return value instanceof RuntimeException;
	}

	@Override
	public Object validateValue( Object value )
	{
		if (validate( value ))
			return value;
		
		throw new IllegalArgumentException( "value not appropriate for RuntimeException: "+value );
	}
}
