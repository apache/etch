/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import java.util.HashSet;
import java.util.Set;

import etch.compiler.ParseException;

/**
 * Abstract base class of annotations from the etch idl.
 */
abstract public class Opt
{
	/**
	 * Constructs the Opt.
	 *
	 * @param name the name of this opt. must be unique.
	 * @param lineno the line number of this opt.
	 */
	public Opt( String name, int lineno )
	{
		this.name = name;
		this.lineno = lineno;
	}
	
	private final String name;
	
	private final int lineno;
	
	private final Set<Class<?>> types = new HashSet<Class<?>>();
	
	/**
	 * @param type adds the specified type to the set of constructs
	 * to which this opt may be attached. generally only called from
	 * the constructor of an opt.
	 */
	public void addType( Class<?> type )
	{
		types.add( type );
	}

	/**
	 * @return the name of this option.
	 */
	public String name()
	{
		return name;
	}
	
	/**
	 * @return the line number of this opt.
	 */
	public int lineno()
	{
		return lineno;
	}

	/**
	 * Dumps the option nicely formatted.
	 * @param indent
	 */
	abstract public void dump( String indent );

	@Override
	abstract public String toString();

	/**
	 * @param type the type of construct to which the opt is attached.
	 * @throws ParseException
	 */
	public void check( Class<?> type ) throws ParseException
	{
		if (types != null)
		{
			if (!types.contains( type ))
				throw new ParseException( String.format(
					"option %s not allowed here, line %d", this, lineno) );
		}
	}
}
