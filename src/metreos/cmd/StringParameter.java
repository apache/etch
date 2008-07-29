/* $Id: StringParameter.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Mar 27, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.cmd;

/**
 * A parameter whose value is a string.
 */
public class StringParameter extends Parameter
{
	/**
	 * @param cp
	 * @param name
	 * @param method
	 * @param description
	 * @param isRequired 
	 * @param constraint
	 * @throws Exception
	 */
	public StringParameter( CommandParser cp, String name, String method,
		String description, boolean isRequired, Constraint constraint ) throws Exception
	{
		super( cp, name, method, PARAMS, description, isRequired, constraint );
	}
	
	private final static Class<?>[] PARAMS =
		{ CommandParser.class, Parameter.class, String.class };

	@Override
	public Object convertValue( String value )
	{
		return value;
	}
}
