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

import java.util.Map;

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.ParseException;


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
