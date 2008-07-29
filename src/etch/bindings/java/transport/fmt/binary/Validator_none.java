package etch.bindings.java.transport.fmt.binary;

import etch.bindings.java.msg.Validator;
import etch.bindings.java.transport.fmt.TypeCode;

/**
 * A validator for the special end of list marker,
 * {@link BinaryTaggedData#NONE}.
 */
public class Validator_none extends Validator
{
	/**
	 * @return the validator for BinaryTaggedData.NONE.
	 */
	public static Validator_none get()
    {
		if (validator == null)
			validator = new Validator_none();
	    
		return validator;
    }
	
	private static Validator_none validator;
	
	/**
	 * Constructs the validator.
	 */
	public Validator_none()
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
	    return value == BinaryTaggedData.NONE;
	}

	@Override
	public Byte checkValue( Object value )
	{
		return validate( value ) ? TypeCode.NONE : null;
	}
}
