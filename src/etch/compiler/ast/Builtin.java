/* $Id$
 *
 * Created by sccomer on Apr 18, 2008.
 *
 * Copyright (c) 2005 Metreos, Inc. All rights reserved.
 */

package etch.compiler.ast;

import etch.compiler.Backend;
import etch.compiler.ParseException;

/**
 * A built-in declaration from the binding.
 */
public class Builtin extends ParamList<Service>
{
	/**
	 * Constructs the Builtin.
	 *
	 * @param intf
	 * @param name
	 * @param bindingName
	 * @param allowSubclass 
	 */
	public Builtin( Service intf, Name name, String bindingName,
		boolean allowSubclass )
	{
		super( intf, name, null );
		this.bindingName = bindingName;
		this.allowSubclass = allowSubclass;
	}
	
	private final String bindingName;
	
	private final boolean allowSubclass;
	
	@Override
	public String toString()
	{
		return String.format( "built-in %s( %s )", name(), bindingName );
	}

	@Override
	public Service service()
	{
		return parent();
	}

	@Override
	public void check()
	{
		// nothing to do.
	}
	
	/**
	 * @return the binding name of this built-in. For example,
	 * Map's binding name for java is java.util.Map<Object, Object>
	 */
	public String bindingName()
	{
		return bindingName;
	}

	/**
	 * @return whether this type allows subclassing or whether the
	 * classes must match exactly.
	 */
	public boolean allowSubclass()
	{
		return allowSubclass;
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%sbuiltin %s: %s\n", indent, name(), bindingName );
	}

	@Override
	public String fqname()
	{
		return bindingName;
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		// nothing to do.
	}

	@Override
	public String vname( Backend helper )
	{
		return null;
	}

	public Object className()
	{
		int i = bindingName.indexOf( '<' );
		if (i >= 0)
			return bindingName.substring( 0, i );
		else
			return bindingName;
	}
}
