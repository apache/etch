/* $Id$
 *
 * Created by wert on Aug 27, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for ArrayValue.
 */
public class Validator_ArrayValue extends TypeValidator
{
	/**
	 * @return an instance of the validator.
	 */
	public static Validator_ArrayValue get()
	{
		if (validator == null)
			validator = new Validator_ArrayValue();
		
		return validator;
	}
	
	private static Validator_ArrayValue validator;
	
	/**
	 * Constructs the Validator.
	 */
	private Validator_ArrayValue()
	{
		super( ArrayValue.class, ArrayValue.class, 0, "ArrayValue" );
	}

	@Override
	public Validator elementValidator()
	{
		return null;
	}
	
	@Override
	public Byte checkValue( Object value )
	{
		if (!validate( value))
			return null;
		
		return TypeCode.ARRAY;
	}
}
