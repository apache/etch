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
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.opt.ToString.FieldItem;
import org.apache.etch.compiler.opt.ToString.FmtItem;
import org.apache.etch.compiler.opt.ToString.StringItem;


/**
 * Abstract base class of service elements which have parameter lists
 * from the etch idl. This includes struct, exception, and message
 * elements.
 * @param <P> the type of the parent object.
 */
abstract public class ParamList<P extends Named<?>> extends Named<P>
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
	
	/**
	 * @param n
	 * @return true if the specified parameter name is defined by this element.
	 */
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
	
	/**
	 * @return true if this element defines any parameters.
	 */
	public boolean hasParameters()
	{
		return !list.isEmpty();
	}

	/**
	 * @return the service that we are part of.
	 */
	abstract public Service service();

	/**
	 * @param isExcept
	 * @param hasExtends
	 * @return a list of format items for this element.
	 */
	public List<FmtItem> mkFormatList( boolean isExcept, boolean hasExtends )
	{
		List<FmtItem> fmts = new ArrayList<FmtItem>();
		
		if (!isExcept)
			fmts.add( new StringItem( name().name+"(" ) );

		if (hasExtends)
		{
			fmts.add( new FieldItem( isExcept ? "super.getMessage()" : "super.toString()" ) );
			fmts.add( new StringItem( "; " ) );
		}
		
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

		return fmts;
	}

	/**
	 * Checks the format items for this element.
	 * @param lineno
	 * @param fmts
	 * @throws ParseException
	 */
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
