/* $Id$
 *
 * Created by wert on Aug 27, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for object.
 */
public class Validator_object extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_object get( int nDims )
	{
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_object( nDims );
		
		Validator_object v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_object( nDims );
		
		return v;
	}
	
	private static Validator_object[] validators = new Validator_object[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_object( int nDims )
	{
		super( Object.class, Object.class, nDims, "object["+nDims+"]", true );
	}

	@Override
	public Validator elementValidator()
	{
		return nDims > 0 ? get( nDims-1 ) : this;
	}
}
