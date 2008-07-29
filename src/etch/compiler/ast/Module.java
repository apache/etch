/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import java.util.Iterator;
import java.util.Map;

import etch.compiler.Backend;
import etch.compiler.ParseException;

/**
 * A module declaration from the etch idl.
 */
public class Module extends Named<Module> implements Iterable<Service>
{
	/**
	 * Constructs the Module.
	 *
	 * @param name this is the moral equivalent of a java package. It is
	 * a dotted name list like "com.cisco.cuae.media".
	 */
	public Module( Name name )
	{
		super( null, name, null );
	}
	
	@Override
	public String vname( Backend helper )
	{
		return null;
	}
	
	@Override
	public String fqname()
	{
		return name().name;
	}
	
//	@Override
//	public Integer id()
//	{
//		return null;
//	}

	/**
	 * @param n
	 * @param opts 
	 * @return the created service.
	 * @throws ParseException 
	 */
	public Service addService( Name n, Map<String, Opt> opts ) throws ParseException
	{
		services.check( n );
		Service i = new Service( this, n, opts );
		services.add( i.name(), i );
		return i;
	}
	
	private NameList<Service> services = new NameList<Service>();


	public Iterator<Service> iterator()
	{
		return services.iterator();
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%smodule %s;\n", indent, name() );
		
		String sIndent = indent+"  ";
		
		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );
		
		for (Service i: this)
			i.dump( sIndent );
	}

	@Override
	public void check() throws ParseException
	{
		checkModuleName( name() );
		
		for (Service i: this)
			i.check();
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.preModule( this );
		
		for (Service s: this)
			s.treewalk( walker );
		
		walker.postModule( this );
	}
}