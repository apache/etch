package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for double.
 */
public class Validator_double extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_double get( int nDims )
	{
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_double( nDims );
		
		Validator_double v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_double( nDims );
		
		return v;
	}
	
	private static Validator_double[] validators = new Validator_double[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_double( int nDims )
	{
		super( Double.class, double.class, nDims, "double["+nDims+"]" );
	}

	@Override
	public Validator elementValidator()
	{
		return get( nDims-1 );
	}
}
