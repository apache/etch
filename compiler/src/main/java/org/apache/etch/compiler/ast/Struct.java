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
import java.util.List;
import java.util.Map;

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.ParseException;


/**
 * A struct declaration from the etch idl.
 */
public class Struct extends ParamList<Service>
{
	/**
	 * Constructs the Struct.
	 *
	 * @param intf
	 * @param name
	 * @param opts
	 */
	public Struct( Service intf, Name name, Map<String, Opt> opts )
	{
		super( intf, name, opts );
		
		moduleName = intf.parent().name().name;
		serviceName = intf.name().name;
		structName = name.name;

		fqname = moduleName+'.'+serviceName+'.'+structName;
		vname = fqname.replace( '.', '_' );
	}
	
	private final String moduleName;
	
	private final String serviceName;
	
	private final String structName;
	
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
		return helper.enum_efqname( fqname, moduleName, serviceName, structName );
	}

	@Override
	public String toString()
	{
		return String.format( "struct %s (%s)", name(), getParameters() );
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%sstruct %s\n", indent, name() );
		
		String sIndent = indent+"  ";
		
		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );
		
		for (Parameter p: this)
			p.dump( sIndent );
	}

	@Override
	public void check() throws ParseException
	{
		checkTypeName( name() );
		
		for (Parameter p: this)
			p.check();
		
		if (hasExtends())
		{
			Named<?> n = parent().get( xtnds.name );
			
			if (n == null)
				throw new ParseException( String.format(
					"struct %s extends %s not defined at line %d",
					name(), xtnds.name(), xtnds.token.beginLine ) );
			
			if (!(n instanceof Struct))
				throw new ParseException( String.format(
					"struct %s extends %s not a struct at line %d",
					name(), xtnds.name(), xtnds.token.beginLine ) );
			
			Struct s = this;
			while (s != null && s.hasExtends())
			{
				s = s.getExtends();
				if (s == this)
					throw new ParseException( String.format(
						"struct %s extends %s creates loop at line %d",
						name(), xtnds.name(), xtnds.token.beginLine ) );
			}
			
			s = getExtends();
			for (Parameter p: this)
			{
				Parameter x = s.getParameter( p.name().name );
				if (x != null)
					throw new ParseException( String.format(
						"struct %s extends %s hides parameter %s from %s at line %d",
						name(), xtnds.name(), x.name(), x.parent().name(),
						xtnds.token.beginLine ) );
			}
		}
	}
	
	@Override
	public Parameter getParameter( String n )
	{
		Parameter p = super.getParameter( n );
		if (p == null && hasExtends())
			p = getExtends().getParameter( n );
		return p;
	}
	
	/**
	 * @return the list of all parameters including those from superclasses.
	 * They are returned in declaration order, starting with those from the
	 * base class.
	 */
	public List<Parameter> getAllParameters()
	{
		if (hasExtends())
		{
			List<Parameter> list = new ArrayList<Parameter>();
			list.addAll( getExtends().getAllParameters() );
			list.addAll( getParameters() );
			return list;
		}
		return getParameters();
	}
	
	/**
	 * @return true if this struct or any extended struct has a parameter.
	 */
	public boolean hasAnyParameters()
	{
		return hasParameters() ||
			hasExtends() && getExtends().hasAnyParameters();
	}

	@Override
	public Service service()
	{
		return parent();
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.preStruct( this );
		
		for (Parameter p: this)
			p.treewalk( walker );
		
		if (hasExtends())
			getExtends().treewalk( walker );
		
		walker.postStruct( this );
	}
	
	/**
	 * @return true if this struct extends another.
	 */
	public boolean hasExtends()
	{
		return xtnds != null;
	}
	
	/**
	 * @return the struct that this one extends or null.
	 */
	public Struct getExtends()
	{
		if (xtnds == null)
			return null;
		
		return (Struct) parent().get( xtnds.name );
	}

	/**
	 * Sets the extends attribute of this exception.
	 * @param xtnds
	 */
	public void setExtends( Name xtnds )
	{
		this.xtnds = xtnds;
	}
	
	private Name xtnds;
}
