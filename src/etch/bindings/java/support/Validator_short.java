package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for short.
 */
public class Validator_short extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_short get( int nDims )
	{
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_short( nDims );
		
		Validator_short v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_short( nDims );
		
		return v;
	}
	
	private static Validator_short[] validators = new Validator_short[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_short( int nDims )
	{
		super( Short.class, short.class, nDims, "short["+nDims+"]" );
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
		
		if (c == Short.class || c == Byte.class)
			return true;
		
		if (c == Integer.class)
		{
			int v = (Integer) value;
			return v >= Short.MIN_VALUE && v <= Short.MAX_VALUE;
		}
		
		if (c == Long.class)
		{
			long v = (Long) value;
			return v >= Short.MIN_VALUE && v <= Short.MAX_VALUE;
		}
		
		return false;
	}
	
	@Override
	public Object validateValue( Object value )
	{
		value = super.validateValue( value );
		
		if (value.getClass() == Short.class || nDims > 0)
			return value;
		
		return ((Number) value).shortValue();
	}
}
