package etch.bindings.java.transport.fmt.binary;

import etch.bindings.java.msg.Validator;

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
		throw new IllegalArgumentException( "not an array" );
    }
	
	@Override
	final public boolean validate( Object value )
	{
	    return value == BinaryTaggedData.NONE;
	}

	@Override
	public Object validateValue( Object value )
	{
		if (validate( value ))
			return value;
		
		throw new IllegalArgumentException( "value not appropriate for none: "+value );
	}
}
