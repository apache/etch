/* $Id$
 *
 * Created by wert on Aug 27, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validates values using one of two validators.
 */
public class ComboValidator extends Validator
{
	/**
	 * Constructs the ComboValidator.
	 *
	 * @param a
	 * @param b
	 */
	public ComboValidator( Validator a, Validator b )
	{
		this.a = a;
		this.b = b;
	}
	
	private final Validator a;
	
	private final Validator b;
	
	@Override
	public String toString()
	{
		return String.format( "(%s OR %s)", a, b );
	}

	@Override
	public Validator elementValidator()
	{
		Validator na;
		try
		{
			na = a.elementValidator();
		}
		catch ( IllegalArgumentException e )
		{
			na = null;
		}
		
		Validator nb;
		try
		{
			nb = b.elementValidator();
		}
		catch ( IllegalArgumentException e )
		{
			nb = null;
		}
		
		if (na == null && nb == null)
			throw new IllegalArgumentException( "na == null && nb == null" );
		
		if (na == null)
			return nb;
		
		if (nb == null)
			return na;
		
		return new ComboValidator( na, nb );
	}

	@Override
	public boolean validate( Object value )
	{
		return a.validate( value ) || b.validate( value );
	}

	@Override
	public Object validateValue( Object value )
	{
		try
		{
			return a.validateValue( value );
		}
		catch (IllegalArgumentException e )
		{
			return b.validateValue( value );
		}
	}

	/**
	 * @return the "a" child of this validator.
	 */
	public Validator a()
	{
		return a;
	}

	/**
	 * @return the "b" child of this validator.
	 */
	public Validator b()
	{
		return b;
	}
}
