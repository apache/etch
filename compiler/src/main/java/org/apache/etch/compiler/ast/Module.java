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
import org.apache.etch.compiler.CmdLineOptions;
import org.apache.etch.compiler.ParseException;


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
	 * @param cmdLineOptions our command line options.
	 */
	public Module( Name name, CmdLineOptions cmdLineOptions )
	{
		super( null, name, null, cmdLineOptions );
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