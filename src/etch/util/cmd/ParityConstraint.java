/* $Id: ParityConstraint.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on May 12, 2005.
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util.cmd;

/**
 * Description of ParityConstraint.
 */
public class ParityConstraint implements Constraint
{
	/**
	 * Constructs the ParityConstraint.
	 *
	 * @param wantsOdd
	 */
	public ParityConstraint( boolean wantsOdd )
	{
		this.wantsOdd = wantsOdd;
	}
	
	private final boolean wantsOdd;
	
	public void checkValue( Object value ) throws Exception
	{
		int v = ((Integer) value).intValue();
		boolean valueIsOdd = ((v & 1) != 0);
		if (valueIsOdd != wantsOdd)
			throw new Exception( wantsOdd ? "is not odd" : "is not even" );
	}

	@Override
	public String toString()
	{
		return wantsOdd ? "must be odd" : "must be even";
	}
}
