/* $Id$
 *
 * Created by sccomer on Mar 29, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import etch.compiler.Token;

/**
 * A name of a thing from the etch idle. If the token is present,
 * it can be used to identify the line number of the name.
 */
public class Name
{
	/**
	 * Constructs the Name.
	 *
	 * @param token
	 * @param name
	 */
	public Name( Token token, String name )
	{
		this.token = token;
		this.name = name;
	}
	
	/**
	 * The original source of the name. Used to get the line number.
	 */
	public final Token token;
	
	/**
	 * The name.
	 */
	public final String name;
	
	/**
	 * @return the string of this name.
	 */
	public String name()
	{
		return name;
	}

	@Override
	public String toString()
	{
		return name;
	}
}