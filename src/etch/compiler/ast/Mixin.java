/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

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
	 */
	public Mixin( Service intf, Name name )
	{
		super( intf, name, null );
	}
	
	@Override
	public String vname( Backend helper )
	{
		return null;
	}
	
	@Override
	public String fqname()
	{
		return null;
	}
	
	@Override
	public Integer id()
	{
		return null;
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%smixin %s\n", indent, name().name );
	}

	@Override
	public void check()
	{
		// TODO Auto-generated method stub
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.doMixin( this );
	}
}
