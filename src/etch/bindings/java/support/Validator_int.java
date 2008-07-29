package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for int.
 */
public class Validator_int extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_int get( int nDims )
	{
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_int( nDims );
		
		Validator_int v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_int( nDims );
		
		return v;
	}
	
	private static Validator_int[] validators = new Validator_int[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_int( int nDims )
	{
		super( Integer.class, int.class, nDims, "int["+nDims+"]" );
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
		
		if (c == Integer.class || c == Short.class || c == Byte.class)
			return true;
		
		if (c == Long.class)
		{
			long v = (Long) value;
			return v >= Integer.MIN_VALUE && v <= Integer.MAX_VALUE;
		}
		
		return false;
	}
	
	@Override
	public Object validateValue( Object value )
	{
		value = super.validateValue( value );
		
		if (value.getClass() == Integer.class || nDims > 0)
			return value;
		
		return ((Number) value).intValue();
	}
}
