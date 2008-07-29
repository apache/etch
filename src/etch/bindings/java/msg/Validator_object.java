/* $Id$
 *
 * Created by wert on Aug 27, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for object.
 */
public class Validator_object extends Validator
{
	@Override
	public Byte checkValue( Object value )
	{
		return TypeCode.ANY;
	}

	@Override
	public Validator elementValidator()
	{
		return this;
	}

	@Override
	public boolean validate( Object value )
	{
		return true;
	}

	public static Validator get( int nDims )
	{
		if (inst == null)
			inst = new Validator_object();
		return inst;
	}
	
	private static Validator_object inst;
}
