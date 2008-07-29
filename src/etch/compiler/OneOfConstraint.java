package etch.compiler;

import java.util.HashSet;
import java.util.Set;

import metreos.cmd.Constraint;

/**
 * A Constraint which limits a value to one of a few
 * choices.
 */
public class OneOfConstraint implements Constraint
{
	/**
	 * Constructs the OneOfConstraint.
	 *
	 * @param value the allowed values
	 */
	public OneOfConstraint( String ... value )
	{
		set = new HashSet<String>();
		for (String s: value)
			set.add( s );
	}

	/**
	 * Constructs the OneOfConstraint.
	 *
	 * @param set the set of allowed values.
	 */
	public OneOfConstraint( Set<String> set )
	{
		this.set = set;
	}
	
	private final Set<String> set;
	
	@Override
	public String toString()
	{
		return "one of "+set;
	}
	
	public void checkValue( Object value ) throws Exception
	{
		if (!set.contains( value ))
			throw new Exception( ""+value+" is not valid, expected one of "+set );
	}
}