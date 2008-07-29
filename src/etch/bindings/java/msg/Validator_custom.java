package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for custom type.
 */
public class Validator_custom extends TypeValidator
{
	/**
	 * @param clss the class of the custom type.
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_custom getCustom( Class<?> clss, int nDims )
    {
		if (nDims < 0)
			return null;
		
		return new Validator_custom( clss, nDims );
    }
	
	/**
	 * Constructs the Validator.
	 *
	 * @param clss the class of the custom type.
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_custom( Class<?> clss, int nDims )
	{
		super( clss, clss, nDims, clss.toString()+"["+nDims+"]" );
		this.clss = clss;
	}
	
	private final Class<?> clss;

	@Override
    public Validator elementValidator()
    {
	    return getCustom( clss, nDims-1 );
    }

	@Override
	public Byte checkValue( Object value )
	{
		if (!validate( value))
			return null;
		
		if (nDims > 0)
			return TypeCode.ARRAY;
		
		return TypeCode.CUSTOM;
	}
}
