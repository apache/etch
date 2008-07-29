/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import etch.bindings.java.msg.IdName;
import etch.compiler.Backend;
import etch.compiler.ParseException;

/**
 * A parameter declaration from the etch idl. A parameter can be
 * a field of a struct or exception, or a parameter of a message.
 */
public class Parameter extends Named<ParamList>
{
	/**
	 * Constructs the Parameter.
	 *
	 * @param pl
	 * @param rt
	 * @param name
	 */
	public Parameter( ParamList pl, Name name, TypeRef rt )
	{
		super( pl, name, null );
		this.type = rt;
		vname = name.name;
		id = IdName.hash( name.name );
	}
	
	private final TypeRef type;
	
	private final String vname;
	
	private final int id;
	
	/**
	 * @return the type of this parameter.
	 */
	public TypeRef type()
	{
		return type;
	}
	
	@Override
	public String vname( Backend helper )
	{
		return helper.mfvname( vname );
	}

	@Override
	public String fqname()
	{
		return name().name;
	}

	@Override
	public Integer id()
	{
		return id;
	}
	
	@Override
	public String toString()
	{
		return type+" "+name().name;
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%sparam %s %s\n", indent, type, name().name );
		
		String sIndent = indent+"  ";
		
		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );
	}

	@Override
	public void check() throws ParseException
	{
		type.check( parent().service() );
	}

	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.preParameter( this );
		
		type.treewalk( walker );
		
		walker.postParameter( this );
	}
}