/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import java.util.Map;

import etch.compiler.Backend;
import etch.compiler.ParseException;

/**
 * A mixin declaration from the etch idl.
 */
public class Mixin extends Named<Service>
{
	/**
	 * Constructs the Mixin.
	 *
	 * @param intf
	 * @param name
	 * @param nOpts 
	 * @param mod 
	 */
	public Mixin( Service intf, Name name, Map<String, Opt> nOpts, Module mod )
	{
		super( intf, name, nOpts );
		this.mod = mod;
	}
	
	private final Module mod;
	
	/**
	 * @return the mixed in module.
	 */
	public Module getModule()
	{
		return mod;
	}
	
	@Override
	public String vname( Backend helper )
	{
		return null;
	}
	
	@Override
	public String fqname()
	{
		return mod.iterator().next().fqname();
	}
	
//	@Override
//	public Integer id()
//	{
//		return null;
//	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%smixin %s\n", indent, name().name );
	}

	@Override
	public void check() throws ParseException
	{
		// nothing to do.
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.doMixin( this );
	}
}
