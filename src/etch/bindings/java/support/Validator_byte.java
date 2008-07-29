package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

/**
 * Validator for byte.
 */
public class Validator_byte extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_byte get( int nDims )
	{
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_byte( nDims );
		
		Validator_byte v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_byte( nDims );
		
		return v;
	}
	
	private static Validator_byte[] validators = new Validator_byte[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_byte( int nDims )
	{
		super( Byte.class, byte.class, nDims, "byte["+nDims+"]" );
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
		
		if (c == Byte.class)
			return true;
		
		if (c == Short.class)
		{
			short v = (Short) value;
			return v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE;
		}
		
		if (c == Integer.class)
		{
			int v = (Integer) value;
			return v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE;
		}
		
		if (c == Long.class)
		{
			long v = (Long) value;
			return v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE;
		}
		
		return false;
	}
	
	@Override
	public Object validateValue( Object value )
	{
		value = super.validateValue( value );
		
		if (value.getClass() == Byte.class || nDims > 0)
			return value;
		
		return ((Number) value).byteValue();
	}
}
