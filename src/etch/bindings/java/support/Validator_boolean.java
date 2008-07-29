package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for boolean.
 */
public class Validator_boolean extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_boolean get( int nDims )
	{
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_boolean( nDims );
		
		Validator_boolean v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_boolean( nDims );
		
		return v;
	}
	
	private static Validator_boolean[] validators = new Validator_boolean[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_boolean( int nDims )
	{
		super( Boolean.class, boolean.class, nDims, "boolean["+nDims+"]" );
	}

	@Override
	public Validator elementValidator()
	{
		return get( nDims-1 );
	}
}
