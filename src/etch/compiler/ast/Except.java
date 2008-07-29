/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import java.util.Map;

import etch.bindings.java.msg.IdName;
import etch.compiler.Backend;
import etch.compiler.ParseException;
import etch.compiler.opt.Unchecked;

/**
 * An exception declaration from the etch idl.
 */
public class Except extends ParamList<Service>
{
	/**
	 * Constructs the Except.
	 *
	 * @param intf
	 * @param name
	 * @param opts 
	 */
	public Except( Service intf, Name name, Map<String, Opt> opts )
	{
		super( intf, name, opts );
		
		moduleName = intf.parent().name().name;
		serviceName = intf.name().name;
		exceptName = name.name;

		fqname = moduleName+'.'+serviceName+'.'+exceptName;
		vname = fqname.replace( '.', '_' );
		id = IdName.hash( fqname );
	}
	
	private final String moduleName;
	
	private final String serviceName;
	
	private final String exceptName;
	
	private final String vname;
	
	private final String fqname;
	
	private final Integer id;
	
	@Override
	public String vname( Backend helper )
	{
		return helper.mtvname( vname );
	}
	
	@Override
	public String fqname()
	{
		return fqname;
	}
	
	@Override
	public String efqname( Backend helper )
	{
		return helper.enum_efqname( fqname, moduleName, serviceName, exceptName );
	}
	
	@Override
	public Integer id()
	{
		return id;
	}

	@Override
	public String toString()
	{
		return String.format( "except %s (%s)", name(), getParameters() );
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%sexception %s\n", indent, name() );
		
		String sIndent = indent+"  ";
		
		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );
		
		for (Parameter p: this)
			p.dump( sIndent );
	}

	@Override
	public void check() throws ParseException
	{
		for (Parameter p: this)
			p.check();
	}

	@Override
	public Service service()
	{
		return parent();
	}
	
	/**
	 * @return returns true if this is an unchecked exception.
	 */
	public boolean isUnchecked()
	{
		Unchecked u = (Unchecked) getOpt( "Unchecked" );
		return u != null && u.isUnchecked();
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.preExcept( this );
		
		for (Parameter p: this)
			p.treewalk( walker );
		
		walker.postExcept( this );
	}
}
