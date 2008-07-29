/* $Id: StringOption.java 21902 2006-04-12 13:28:38Z wert $
 *
 * Created by wert on Mar 27, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.cmd;

/**
 * An option whose value is a string.
 */
public class StringArrayOption extends Option
{
	/**
	 * @param cp
	 * @param tokens
	 * @param names
	 * @param method
	 * @param description
	 * @param flags
	 * @param defaultValues
	 * @param constraints
	 * @throws Exception
	 */
	public StringArrayOption( CommandParser cp, String tokens, String[] names,
		String method, String description, int flags, String[] defaultValues,
		Constraint[] constraints ) throws Exception
	{
		super( cp, tokens, names, method, PARAMS, description, flags,
			defaultValues, constraints );
	}
	
	private final static Class<?>[] PARAMS =
		{ CommandParser.class, Option.class, String.class, Object[].class };

	@Override
	protected Object convertValue( String value )
	{
		return value;
	}
}
