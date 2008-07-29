/* $Id$
 *
 * Created by wert on Aug 27, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

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
		return String.format( "(%s) OR (%s)", a, b );
	}

	@Override
	public Validator elementValidator()
	{
		Validator na = a.elementValidator();
		Validator nb = b.elementValidator();
		
		if (na == null && nb == null)
			return null;
		
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
	public Byte checkValue( Object value )
	{
		Byte type = a.checkValue( value );
		if (type == null)
			type = b.checkValue( value );
		return type;
	}
}
