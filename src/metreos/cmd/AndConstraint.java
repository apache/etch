/* $Id: AndConstraint.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on May 12, 2005.
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.cmd;

/**
 * Description of AndConstraint.
 */
public class AndConstraint implements Constraint
{
	/**
	 * Constructs the AndConstraint.
	 * @param a 
	 * @param b 
	 */
	public AndConstraint( Constraint a, Constraint b )
	{
		this.a = a;
		this.b = b;
	}
	
	private final Constraint a;
	
	private final Constraint b;
	
	public void checkValue( Object value ) throws Exception
	{
		a.checkValue( value );
		b.checkValue( value );
	}
	
	@Override
	public String toString()
	{
		return "("+a+") AND ("+b+")";
	}
}
