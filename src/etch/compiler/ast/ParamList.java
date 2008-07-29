/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import etch.compiler.ParseException;
import etch.compiler.opt.ToString.FieldItem;
import etch.compiler.opt.ToString.FmtItem;
import etch.compiler.opt.ToString.StringItem;

/**
 * Abstract base class of service elements which have parameter lists
 * from the etch idl. This includes struct, exception, and message
 * elements.
 * @param <P> the type of the parent object.
 */
abstract public class ParamList<P extends Named> extends Named<P>
	implements Iterable<Parameter>
{
	/**
	 * Constructs the ParamList.
	 *
	 * @param parent
	 * @param name
	 * @param opts 
	 */
	public ParamList( P parent, Name name, Map<String, Opt> opts )
	{
		super( parent, name, opts );
	}

	/**
	 * @param rt
	 * @param n
	 * @return the parameter that was added.
	 * @throws ParseException 
	 */
	public Parameter addParameter( Name n, TypeRef rt ) throws ParseException
	{
//		if (rt.type().kind == EtchGrammarConstants.VOID)
//			throw new IllegalArgumentException( "bad parameter type "+rt+" for name "+n );
		
		list.check( n );
		Parameter p = new Parameter( this, n, rt );
		list.add( n, p );
		return p;
	}
	
	public boolean hasParameter( String n )
	{
		return list.has( n );
	}

	/**
	 * @param n the parameter name to get.
	 * @return the parameter with the specified name, or null.
	 */
	public Parameter getParameter( String n )
	{
		return list.get( n );
	}
	
	/**
	 * @return a list containing the parameters in order.
	 */
	public List<Parameter> getParameters()
	{
		return list.list();
	}
	
	private NameList<Parameter> list = new NameList<Parameter>();

	public Iterator<Parameter> iterator()
	{
		return list.iterator();
	}
	
	public boolean hasParameters()
	{
		return !list.isEmpty();
	}

	/**
	 * @return the service that we are part of.
	 */
	abstract public Service service();

	public List<FmtItem> mkFormatList( boolean isExcept )
	{
		List<FmtItem> fmts = new ArrayList<FmtItem>();
//		fmts.add( new StringItem( "\"" ) );
		if (!isExcept)
			fmts.add( new StringItem( name().name+"(" ) );
		boolean first = true;
		for (Parameter p: list)
		{
			if (first)
			{
				first = false;
				fmts.add( new StringItem( p.name().name+"=" ) );
			}
			else
			{
				fmts.add( new StringItem( "; "+p.name().name+"=" ) );
			}
			fmts.add( new FieldItem( p.name().name ) );
		}
		if (!isExcept)
			fmts.add( new StringItem( ")" ) );
//		fmts.add( new StringItem( "\"" ) );
		return fmts;
	}

	public void checkFormatList( int lineno, List<FmtItem> fmts )
		throws ParseException
	{
		for (FmtItem i: fmts)
		{
			if (i instanceof FieldItem)
			{
				FieldItem fi = (FieldItem) i;
				if (!hasParameter( fi.value() ))
					throw new ParseException( String.format(
						"ToString: format list specifies parameter (%s) which does not exist at line %d",
						fi.value(), lineno ) );
			}
		}
	}
}
