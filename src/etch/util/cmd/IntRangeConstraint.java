/* $Id: IntRangeConstraint.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Apr 29, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util.cmd;

/**
 * Description of IntRangeConstraint
 */
public class IntRangeConstraint implements Constraint
{
	/**
	 * Constructs the IntRangeConstraint.
	 *
	 * @param min the minimum value allowed.
	 * 
	 * @param max the maximum value allowed.
	 */
	public IntRangeConstraint( int min, int max )
	{
		if (min > max)
			throw new IllegalArgumentException( "min > max" );
		this.min = min;
		this.max = max;
	}
	
	private final int min;
	
	private final int max;
	
	public void checkValue( Object value ) throws Exception
	{
		int v = ((Integer) value).intValue();
		if (v < min || v > max)
			throw new Exception( "value not in range "+min+" to "+max );
	}
	
	@Override
	public String toString()
	{
		return "value must be in range "+min+" to "+max;
	}
}
