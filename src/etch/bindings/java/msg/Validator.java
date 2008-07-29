package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validate values put into messages.
 */
abstract public class Validator
{
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
	 * @return the appropriate type code for this value or null
	 * if this validator does not recognise the value.
	 * @see TypeCode
	 */
	abstract public Byte checkValue( Object value );
}
