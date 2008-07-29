package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for float.
 */
public class Validator_float extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_float get( int nDims )
	{
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_float( nDims );
		
		Validator_float v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_float( nDims );
		
		return v;
	}
	
	private static Validator_float[] validators = new Validator_float[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_float( int nDims )
	{
		super( Float.class, float.class, nDims, "float["+nDims+"]" );
	}

	@Override
	public Validator elementValidator()
	{
		return get( nDims-1 );
	}
}
