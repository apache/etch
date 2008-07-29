package etch.bindings.java.support;

import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator;

/**
 * Validator for StructValue.
 */
public class Validator_StructValue extends TypeValidator
{
	/**
	 * @param type the expected type of a scalar StructValue.
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_StructValue get( Type type, int nDims )
	{
		checkDims( nDims );
		
//		if (nDims >= validators.length)
			return new Validator_StructValue( type, nDims );
		
//		Validator_StructValue v = validators[nDims];
//		
//		if (v == null)
//			v = validators[nDims] = new Validator_StructValue( type, nDims );
//		
//		return v;
	}
	
//	private static Validator_StructValue[] validators = new Validator_StructValue[MAX_CACHED];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_StructValue( Type type, int nDims )
	{
		super( StructValue.class, StructValue.class, nDims, "StructValue["+type+", "+nDims+"]" );
		this.type = type;
	}
	
	private final Type type;

	/**
	 * @return the type of this validator.
	 */
	public Type getType()
	{
		return type;
	}

	@Override
	public Validator elementValidator()
	{
		return get( type, nDims-1 );
	}

	@Override
	public boolean validate( Object value )
	{
		return super.validate( value ) &&
			(value.getClass() != StructValue.class ||
				type.isAssignableFrom( ((StructValue) value).type() ) );
	}
}
