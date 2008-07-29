/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import java.util.Iterator;
import java.util.Map;

import etch.bindings.java.msg.IdName;
import etch.compiler.Backend;
import etch.compiler.ParseException;

/**
 * An enumeration (named set of named constants) declaration from the etch idl.
 */
public class Enumx extends Named<Service> implements Iterable<Item>
{
	/**
	 * Constructs the Enumx.
	 *
	 * @param intf
	 * @param name
	 * @param nOpts 
	 */
	public Enumx( Service intf, Name name, Map<String, Opt> nOpts )
	{
		super( intf, name, nOpts );
		
		moduleName = intf.parent().name().name;
		serviceName = intf.name().name;
		enumName = name.name;

		fqname = moduleName+'.'+serviceName+'.'+enumName;
		vname = fqname.replace( '.', '_' );
		id = IdName.hash( fqname );
	}
	
	private final String moduleName;
	
	private final String serviceName;
	
	private final String enumName;
	
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
		return helper.enum_efqname( fqname, moduleName, serviceName, enumName );
	}
	
	@Override
	public Integer id()
	{
		return id;
	}

	/**
	 * Adds a item to the enumeration.
	 * @param name
	 * @return the item added.
	 * @throws ParseException 
	 */
	public Item addItem( Name name ) throws ParseException
	{
		list.check( name );
		Item i = new Item( this, name );
		list.add( i.name(), i );
		return i;
	}
	
	private final NameList<Item> list = new NameList<Item>();

	@Override
	public String toString()
	{
		return String.format( "enum %s %s", name(), list );
	}
	
	@Override
	public void dump( String indent )
	{
		System.out.printf( "%senum %s\n", indent, name() );
		
		String sIndent = indent+"  ";
		
		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );
		
		for (Item i: list)
			i.dump( sIndent );
	}

	@Override
	public void check()
	{
		for (Item i: list)
			i.check();
	}

	public Iterator<Item> iterator()
	{
		return list.iterator();
	}

	/**
	 * @param name
	 * @return the enum item.
	 */
	public Item getItem( String name )
	{
		return list.get( name );
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.preEnum( this );
		
		for (Item i: this)
			i.treewalk( walker );
		
		walker.postEnum( this );
	}
}
