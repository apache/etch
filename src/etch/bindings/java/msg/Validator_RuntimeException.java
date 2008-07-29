/* $Id$
 *
 * Created by wert on Aug 20, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for RuntimeExceptions.
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
	 *
	 */
	public Validator_RuntimeException()
	{
		// nothing to do.
	}

	@Override
    public Validator elementValidator()
    {
	    return null;
    }

	@Override
	final public boolean validate( Object value )
	{
		return value instanceof RuntimeException;
	}

	@Override
	public Byte checkValue( Object value )
	{
		return validate( value ) ? TypeCode.CUSTOM : null;
	}
}
