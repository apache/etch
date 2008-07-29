package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for void.
 */
public class Validator_void extends Validator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_void get( int nDims )
    {
		if (nDims != 0)
			throw new IllegalArgumentException( "nDims != 0" );
		
		if (validator == null)
			validator = new Validator_void();
		
		return validator;
    }
	
	private static Validator_void validator;
	
	/**
	 * Constructs the Validator.
	 */
	private Validator_void()
	{
		// nothing to do.
	}

	@Override
    public Validator elementValidator()
    {
	    return null;
    }

	@Override
	final public boolean validate( Object value )
	{
		return value == null;
	}

	@Override
	public Object validateValue( Object value )
	{
		if (validate( value ))
			return value;
		
		throw new IllegalArgumentException( "value not appropriate for void: "+value );
	}
}
