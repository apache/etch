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

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.CmdLineOptions;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.Token;


/**
 * Abstraction of a named item from the etch idl.
 * @param <P> the parent type of this item.
 */
abstract public class Named<P extends Named<?>>
{
	/**
	 * Constructs the Named.
	 *
	 * @param parent
	 * @param name
	 * @param nOpts
	 */
	public Named( P parent, Name name, Map<String, Opt> nOpts )
	{
		this( parent, name, nOpts, null );
	}
	
	/**
	 * Constructs the Named.
	 *
	 * @param parent
	 * @param name
	 * @param nOpts
	 * @param cmdLineOptions 
	 */
	public Named( P parent, Name name, Map<String, Opt> nOpts,
		CmdLineOptions cmdLineOptions )
	{
		this.parent = parent;
		this.name = name;
		this.nOpts = nOpts;
		this.cmdLineOptions = cmdLineOptions;
	}
	
	private final P parent;
	
	private final Name name;
	
	private final Map<String, Opt> nOpts;
	
	private final CmdLineOptions cmdLineOptions;
	
	/**
	 * @return the parent of this item.
	 */
	public P parent()
	{
		return parent;
	}
	
	/**
	 * @return the name of this item.
	 */
	public Name name()
	{
		return name;
	}
	
	/**
	 * @param n the opt name to check.
	 * @return true if the opt is present here or in the parent.
	 */
	public boolean hasOpt( String n )
	{
		return getOpt( n ) != null;
	}
	
	/**
	 * @return the command line options of us, or those of the parent.
	 */
	public CmdLineOptions getCmdLineOptions()
	{
		if (cmdLineOptions != null)
			return cmdLineOptions;
		
		return parent != null ? parent.getCmdLineOptions() : null;
	}
	
	/**
	 * @param n the name of the opt.
	 * @return the opt if found. if this named does not define
	 * it, search the parent.
	 */
	public Opt getOpt( String n )
	{
		Opt opt = null;
		if (nOpts != null)
			opt = nOpts.get( n );
		if (opt == null && parent != null)
			opt = parent.getOpt( n );
		return opt;
	}
	
	/**
	 * @return the opts of this named thing.
	 */
	public Collection<Opt> opts()
	{
		return nOpts.values();
	}
	
	/**
	 * @param helper backend to help with name formats
	 * @return the variable name of this named.
	 */
	abstract public String vname( Backend helper );
	
	/**
	 * @return the fully qualified name of this named (as used in Type
	 * declaration).
	 */
	abstract public String fqname();

	/**
	 * @param helper
	 * @return the fully qualified enum, struct, or extern name to use
	 * when refering to the item in code. most probably the same as fqname
	 * except for extern.
	 */
	public String efqname( Backend helper )
	{
		return fqname();
	}
	
	/**
	 * @return true if there is a description.
	 */
	public boolean hasDescr()
	{
		return descr != null && !descr.isEmpty();
	}
	
	/**
	 * @return the module description.
	 */
	public List<String> descr()
	{
		return descr;
	}

	/**
	 * @param descr
	 */
	public void setDescr( List<String> descr )
	{
		this.descr = descr;
	}
	
	private List<String> descr = new ArrayList<String>();

	/**
	 * Dumps the item nicely formatted.
	 * @param indent
	 */
	abstract public void dump( String indent );

	/**
	 * Checks the item for being fully defined.
	 * @throws ParseException
	 */
	abstract public void check() throws ParseException;

	protected void checkModuleName( Name name )
	{
		boolean ok = true;
		
		StringTokenizer st = new StringTokenizer( name.name, "." );
		while (ok && st.hasMoreTokens())
			ok = startsWithLower( st.nextToken() );
		
		if (!ok)
			warn(
				"module element name should start with lowercase",
				name.name, name.token );
	}

	protected void checkConstantName( Name name )
	{
		if (!name.name.toUpperCase().equals( name.name ))
			warn(
				"constant name should be all uppercase",
				name.name, name.token );
	}

	protected void checkTypeName( Name name )
	{
		if (!startsWithUpper( name.name ))
			warn(
				"type name should start with uppercase",
				name.name, name.token );
	}

	protected void checkParamName( Name name )
	{
		if (!startsWithLower( name.name ))
			warn(
				"parameter name should start with lowercase",
				name.name, name.token );
	}

	protected void checkMessageName( Name name )
	{
		if (!startsWithLower( name.name ))
			warn(
				"message name should start with lowercase",
				name.name, name.token );
	}
	
	private void warn( String msg, String name, Token token )
	{
		// TODO turn on warnings for naming conventions.
//		System.err.printf( "Warning: %s: %s at line %d\n",
//			msg, name, token != null ? token.beginLine : 0 );
	}
	
	private boolean startsWithLower( String s )
	{
		return Character.isLowerCase( s.charAt( 0 ) ) || s.charAt( 0 ) == '_';
	}

	private boolean startsWithUpper( String s )
	{
		return Character.isUpperCase( s.charAt( 0 ) );
	}
	
	/**
	 * Walks the tree from the top to the bottom, visiting every node.
	 * @param walker called with each and every node under this one.
	 * @throws ParseException
	 */
	abstract public void treewalk( TreeWalker walker ) throws ParseException;
	
	/**
	 * @return true if this is a constant.
	 */
	public boolean isConstant() { return this instanceof Constant; }
	
	/**
	 * @return true if this is an enum.
	 */
	public boolean isEnumx() { return this instanceof Enumx; }
	
	/**
	 * @return true if this is a struct.
	 */
	public boolean isStruct() { return this instanceof Struct; }
	
	/**
	 * @return true if this is an extern.
	 */
	public boolean isExtern() { return this instanceof Extern; }
	
	/**
	 * @return true if this is a built-in.
	 */
	public boolean isBuiltin() { return this instanceof Builtin; }
	
	/**
	 * @return true if this is an exception.
	 */
	public boolean isExcept() { return this instanceof Except; }
	
	/**
	 * @return true if this is a message.
	 */
	public boolean isMessage() { return this instanceof Message; }
	
	/**
	 * @return true if this is a Mixin
	 */
	
	public boolean isMixin() { return this instanceof Mixin; }
}
