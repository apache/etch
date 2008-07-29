package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for long.
 */
public class Validator_long extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_long get( int nDims )
	{
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_long( nDims );
		
		Validator_long v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_long( nDims );
		
		return v;
	}
	
	private static Validator_long[] validators = new Validator_long[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_long( int nDims )
	{
		super( Long.class, long.class, nDims, "long["+nDims+"]" );
	}

	@Override
	public Validator elementValidator()
	{
		return get( nDims-1 );
	}
	
	@Override
	public boolean validate( Object value )
	{
		if (nDims > 0)
			return super.validate( value );
		
		if (value == null)
			return false;
		
		Class<?> c = value.getClass();
		
		return c == Long.class || c == Integer.class ||
			c == Short.class || c == Byte.class;
	}
	
	@Override
	public Object validateValue( Object value )
	{
		value = super.validateValue( value );
		
		if (value.getClass() == Long.class || nDims > 0)
			return value;
		
		return ((Number) value).longValue();
	}
}
