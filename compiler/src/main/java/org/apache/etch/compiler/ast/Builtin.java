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

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.ParseException;


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

	/**
	 * @return the name of the builtin class.
	 */
	public String className()
	{
		return bindingName;
	}
}
