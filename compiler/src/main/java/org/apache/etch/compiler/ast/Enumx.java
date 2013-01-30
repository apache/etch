/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.compiler.ast;

import java.util.Iterator;
import java.util.Map;

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.ParseException;


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
	}
	
	private final String moduleName;
	
	private final String serviceName;
	
	private final String enumName;
	
	private final String vname;
	
	private final String fqname;
	
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
		checkTypeName( name() );
		
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
