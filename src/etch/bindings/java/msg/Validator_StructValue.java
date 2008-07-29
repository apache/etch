/* $Id$
 *
 * Created by wert on Aug 27, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for StructValue.
 */
public class Validator_StructValue extends TypeValidator
{
	/**
	 * @return an instance of the validator.
	 */
	public static Validator_StructValue get()
	{
		if (validator == null)
			validator = new Validator_StructValue();
		
		return validator;
	}
	
	private static Validator_StructValue validator;
	
	/**
	 * Constructs the Validator.
	 */
	private Validator_StructValue()
	{
		super( StructValue.class, StructValue.class, 0, "StructValue" );
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
		
		return TypeCode.STRUCT;
	}
}
