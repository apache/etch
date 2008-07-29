package etch.bindings.java.msg;

/**
 * Validate values put into messages.
 */
abstract public class Validator
{
	/**
	 * The maximum number of dimensions for arrays (9).
	 */
	public final static int MAX_NDIMS = 9;
	
	/**
	 * The maximum number of validators to cache per type.
	 */
	protected static final int MAX_CACHED = 4;
	
	/**
	 * @return for an array type, return a validator for an element of
	 * the array (which might be a scalar or another array element).
	 */
	abstract public Validator elementValidator();

	/**
	 * Checks the value for being valid. If the value is valid
	 * by this validator, return true.
	 * @param value the value to be validated.
	 * @return true if the value is valid by any validator in the
	 * chain, or false if all reject it.
	 */
	abstract public boolean validate( Object value );

	/**
	 * @param value
	 * @return the appropriate value given the input value and this
	 * validator's sensibility about what it should be really (e.g.,
	 * input Byte but output Long.
	 */
	abstract public Object validateValue( Object value );
}
