/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import java.util.ArrayList;
import java.util.Collection;
import java.util.List;
import java.util.Map;

import etch.compiler.Backend;
import etch.compiler.ParseException;

/**
 * Abstraction of a named item from the etch idl.
 * @param <P> the parent type of this item.
 */
abstract public class Named<P extends Named>
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
		this.parent = parent;
		this.name = name;
		this.nOpts = nOpts;
	}
	
	private final P parent;
	
	private final Name name;
	
	private final Map<String, Opt> nOpts;
	
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
	 * @return the id of this named.
	 */
	abstract public Integer id();
	
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
	 * @return true if this is an struct.
	 */
	public boolean isStruct() { return this instanceof Struct; }
	
	/**
	 * @return true if this is an struct.
	 */
	public boolean isExtern() { return this instanceof Extern; }
	
	/**
	 * @return true if this is an exception.
	 */
	public boolean isExcept() { return this instanceof Except; }
	
	/**
	 * @return true if this is a message.
	 */
	public boolean isMessage() { return this instanceof Message; }
}
