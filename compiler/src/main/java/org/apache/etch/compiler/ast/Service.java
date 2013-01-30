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
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import org.apache.etch.compiler.Backend;
import org.apache.etch.compiler.EtchCompiler;
import org.apache.etch.compiler.EtchGrammarConstants;
import org.apache.etch.compiler.ParseException;
import org.apache.etch.compiler.Token;



/**
 * Declaration of a service from the etch idl.
 */
public class Service extends Named<Module> implements OptList, Iterable<Named<?>>
{
	/**
	 * Constructs the service.
	 *
	 * @param module
	 * @param name
	 * @param opts
	 */
	public Service( Module module, Name name, Map<String, Opt> opts )
	{
		super( module, name, opts );
	}
	
	@Override
	public String vname( Backend helper )
	{
		return null;
	}
	
	@Override
	public String fqname()
	{
		return parent().fqname() + '.' + name().name;
	}
	
//	@Override
//	public Integer id()
//	{
//		return null;
//	}

	/**
	 * Adds a mixin declaration.
	 * 
	 * @param n
	 * @param nOpts
	 * @return the added mixin.
	 * @throws ParseException
	 */
	public Mixin addMixin( Name n, Map<String, Opt> nOpts )
		throws ParseException
	{
		optsCheck( Mixin.class, nOpts );
		
		try
		{
			Module module = EtchCompiler.parseModule( this, n, nOpts );
			if (module == null)
				throw new ParseException( String.format(
					"could not find mixin '%s' at line %d",
					n.name, n.token.beginLine ) );
			
			Mixin mixin = new Mixin( this, n, nOpts, module );
			nameList.add( n, mixin );
			return mixin;
		}
		catch ( ParseException e )
		{
			throw e;
		}
		catch ( Exception e )
		{
			throw new ParseException( String.format(
				"could not find mixin '%s' at line %d: %s",
				n.name, n.token.beginLine, e ) );
		}
	}
	
	/**
	 * Adds a constant declaration.
	 * @param n
	 * @param nOpts
	 * @param tr
	 * @param value
	 * @return the added constant.
	 * @throws ParseException
	 */
	public Constant addConstant( Name n, Map<String, Opt> nOpts, TypeRef tr, Token value ) throws ParseException
	{
		nameList.check( n );
		optsCheck( Constant.class, nOpts );
		Constant c = new Constant( this, n, nOpts, tr, value );
		nameList.add( n, c );
		return c;
	}

	private void optsCheck( Class<?> type, Map<String, Opt> nOpts ) throws ParseException
	{
		for (Opt opt: nOpts.values())
			optCheck( type, opt );
	}

	private void optCheck( Class<?> type, Opt opt ) throws ParseException
	{
		opt.check( type );
	}

	/**
	 * Adds an enumeration declaration.
	 * @param n
	 * @param nOpts
	 * @return the added enumx.
	 * @throws ParseException
	 */
	public Enumx addEnum( Name n, Map<String, Opt> nOpts ) throws ParseException
	{
		nameList.check( n );
		optsCheck( Enumx.class, nOpts );
		Enumx e = new Enumx( this, n, nOpts );
		nameList.add( n, e );
		return e;
	}

	/**
	 * Adds a struct declaration.
	 * @param n
	 * @param nOpts
	 * @return the added struct.
	 * @throws ParseException
	 */
	public Struct addStruct( Name n, Map<String, Opt> nOpts ) throws ParseException
	{
		nameList.check( n );
		optsCheck( Struct.class, nOpts );
		Struct s = new Struct( this, n, nOpts );
		nameList.add( n, s );
		return s;
	}

	/**
	 * Adds an extern declaration.
	 * @param n
	 * @param nOpts
	 * @return the added extern.
	 * @throws ParseException
	 */
	public Extern addExtern( Name n, Map<String, Opt> nOpts ) throws ParseException
	{
		nameList.check( n );
		optsCheck( Extern.class, nOpts );
		Extern s = new Extern( this, n, nOpts );
		nameList.add( n, s );
		return s;
	}

	/**
	 * Adds a built-in declaration.
	 * @param n
	 * @param bindingName
	 * @param allowSubclass
	 * @return the Builtin that was added.
	 * @throws ParseException
	 */
	public Builtin addBuiltin( Name n, String bindingName, boolean allowSubclass )
		throws ParseException
	{
		nameList.check( n );
		Builtin b = new Builtin( this, n, bindingName, allowSubclass );
		nameList.add( n, b );
		return b;
	}

	/**
	 * Adds an exception declaration.
	 * @param n
	 * @param nOpts
	 * @return the added exception.
	 * @throws ParseException
	 */
	public Except addExcept( Name n, Map<String, Opt> nOpts ) throws ParseException
	{
		nameList.check( n );
		optsCheck( Except.class, nOpts );
		Except e = new Except( this, n, nOpts );
		nameList.add( n, e );
		return e;
	}

	/**
	 * Adds a message declaration.
	 * @param n
	 * @param rt
	 * @param nOpts
	 * @return the added message.
	 * @throws ParseException
	 */
	public Message addMessage( Name n, Map<String, Opt> nOpts, TypeRef rt ) throws ParseException
	{
		nameList.check( n );
		optsCheck( Message.class, nOpts );
		Message m = new Message( this, n, nOpts, rt );
		nameList.add( n, m );
		
		if (!m.isOneway())
			addResultMessage( m );
		else if (m.hasReturn())
			throw new ParseException( String.format(
				"Oneway message cannot have a non-void return type at line %d", n.token.beginLine ) );
		
		return m;
	}
	
	/**
	 * @param reqMsg
	 * @throws ParseException
	 */
	public void addResultMessage( Message reqMsg ) throws ParseException
	{
		if (reqMsg.getResultMessage() != null)
			return;
		
		Name rmName = reqMsg.getResultMessageName();

		Token rmType = new Token();
		rmType.kind = EtchGrammarConstants.VOID;
		rmType.image = "void";
		TypeRef rmTypeRef = new TypeRef( this, rmType );
		
		nameList.check( rmName );
		Message rm = new Message( this, rmName, new HashMap<String, Opt>(),
			rmTypeRef );
		rm.setMsgDir( getResultMessageDirection( reqMsg.msgDir() ) );
		nameList.add( rmName, rm );
		
		rm.addParameter( reqMsg.getResultParamName(), reqMsg.type() );
		
		reqMsg.setResultMessage( rm );
		rm.setRequestMessage( reqMsg );
	}

	/**
	 * @param md
	 * @return message direction opposite to the supplied message direction.
	 */
	public MessageDirection getResultMessageDirection( MessageDirection md )
	{
		switch (md)
		{
			case CLIENT: return MessageDirection.SERVER;
			case SERVER: return MessageDirection.CLIENT;
			case BOTH: return MessageDirection.BOTH;
			default: throw new IllegalArgumentException( "unknown message direction "+md );
		}
	}

	public Iterator<Named<?>> iterator()
	{
		return nameList.iterator();
	}
	
	/**
	 * @param recursive if true, recurse into mixed in services, too.
	 * @return an iterator over all the methods of this service.
	 */
	public Iterator<Named<?>> messages( boolean recursive )
	{
		List<Named<?>> list = new ArrayList<Named<?>>();
		getMessages( list, recursive );
		return list.iterator();
	}
	
	private void getMessages( List<Named<?>> list, boolean recursive )
	{
		for (Named<?> n: nameList)
		{
			if (n.isMessage())
				list.add( n );
			else if (recursive && n.isMixin())
				((Mixin) n).getModule().iterator().next().getMessages( list, recursive );
		}
	}

	/**
	 * @param recursive if true, recurse into mixed in services, too
	 * @return an iterator over all the structs of this service
	 */
	public Iterator<Named<?>> structs( boolean recursive )
	{
		List<Named<?>> list = new ArrayList<Named<?>>();
		getStructs( list, recursive );
		return list.iterator();
	}
	
	private void getStructs( List<Named<?>> list, boolean recursive )
	{
		for ( Named<?> n: nameList )
		{
			if ( n.isStruct() )
				list.add(n);
			else if ( recursive && n.isMixin() )
				((Mixin)n).getModule().iterator().next().getStructs( list, recursive );
		}
	}
	
	/**
	 * @param recursive if true, recurse into mixed in services, too
	 * @return an iterator over all the enums of this service
	 */
	public Iterator<Named<?>> enums( boolean recursive )
	{
		List<Named<?>> list = new ArrayList<Named<?>>();
		getEnums( list, recursive );
		return list.iterator();
	}
	
	private void getEnums( List<Named<?>> list, boolean recursive )
	{
		for ( Named<?> n: nameList )
		{
			if ( n.isEnumx() )
				list.add(n);
			else if ( recursive && n.isMixin() )
				((Mixin)n).getModule().iterator().next().getEnums( list, recursive );
		}
	}
	
	/**
	 * @param recursive if true, recurse into mixed in services, too
	 * @return an iterator over all the externs of this service
	 */
	public Iterator<Named<?>> externs( boolean recursive )
	{
		List<Named<?>> list = new ArrayList<Named<?>>();
		getExterns( list, recursive );
		return list.iterator();
	}
	
	private void getExterns( List<Named<?>> list, boolean recursive )
	{
		for ( Named<?> n: nameList )
		{
			if ( n.isExtern() )
				list.add(n);
			else if ( recursive && n.isMixin() )
				((Mixin)n).getModule().iterator().next().getExterns( list, recursive );
		}
	}
	
	/**
	 * @param recursive if true, recurse into mixed in services, too
	 * @return an iterator over all the constants of this service
	 */
	public Iterator<Named<?>> constants( boolean recursive )
	{
		List<Named<?>> list = new ArrayList<Named<?>>();
		getConstants( list, recursive );
		return list.iterator();
	}
	
	private void getConstants( List<Named<?>> list, boolean recursive )
	{
		for ( Named<?> n: nameList )
		{
			if ( n.isConstant() )
				list.add(n);
			else if ( recursive && n.isMixin() )
				((Mixin)n).getModule().iterator().next().getConstants( list, recursive );
		}
	}
	
	/**
	 * @param recursive if true, recurse into mixed in services, too
	 * @return an iterator over all the exceptions of this service
	 */
	public Iterator<Named<?>> exceptions( boolean recursive )
	{
		List<Named<?>> list = new ArrayList<Named<?>>();
		getExceptions( list, recursive );
		return list.iterator();
	}
	
	private void getExceptions( List<Named<?>> list, boolean recursive )
	{
		for ( Named<?> n: nameList )
		{
			if ( n.isExcept() )
				list.add(n);
			else if ( recursive && n.isMixin() )
				((Mixin)n).getModule().iterator().next().getExceptions( list, recursive );
		}
	}
	
	private final NameList<Named<?>> nameList = new NameList<Named<?>>();

	/**
	 * @param mc
	 * @return true if some of that type exists.
	 */
	public boolean hasMessageDirection( MessageDirection mc )
	{
		for (Named<?> n: this)
		{
			//System.out.printf( "hasMessageDirection (%s): %s\n", mc, n );
			
			if (!(n instanceof Message))
			{
				if (mc == MessageDirection.BOTH)
					return true;
				
				continue;
			}
			
			Message m = (Message) n;
			if (m.msgDir() == mc && !m.isHidden())
				return true;
		}
		return false;
	}

	@Override
	public void dump( String indent )
	{
		System.out.printf( "%sservice %s\n", indent, name() );
		
		String sIndent = indent+"  ";
		
		for (String s: descr())
			System.out.printf( "%sdescr %s\n", sIndent, s );
		
		for (Opt o: opts())
			o.dump( sIndent );
		
		for (Named<?> m: this)
			m.dump( sIndent );
	}

	@Override
	public void check() throws ParseException
	{
		checkTypeName( name() );
		
		for (Named<?> n: this)
			n.check();
	}

	/**
	 * Gets the named thing from the service.
	 * @param name
	 * @return the named thing, or null.
	 */
	public Named<?> get( String name )
	{
		List<Named<?>> list = new ArrayList<Named<?>>();
	
		Named<?> named = nameList.get( name );
		if (named != null)
		{
			/*System.out.println( "resolved to "+named );
			return named;*/
			list.add(named);
		}
		
		if (isQualifiedName( name ))
		{
			String x = getSimpleName( name );
			// System.out.println( "trying "+x );
			named = nameList.get( x );
			if (named != null)
			{
				String y = named.fqname();
				if (name.equals( y ))
				{
					list.add(named);
				}
			}
		}


		for (Named<?> n : this)
		{
			if (n.isMixin())
			{

				named = ((Mixin) n).getModule().iterator().next().get( name );
				
				if (named != null)
				//	return named;
				{
					boolean exists = false;
					Iterator<Named<?>> it = list.iterator();
					while ( it.hasNext())
					{
						if ( ((Named<?>)it.next()).fqname().equals( named.fqname() ) )
						{
							exists = true;
						}
					}
					
					if ( !exists )
						list.add(named);
				}

			}
		}

		if (list.size() == 1 )
			return list.get( 0 );


		return null;
	}

	private boolean isQualifiedName( String name )
	{
		return name.lastIndexOf( '.' ) >= 0;
	}

	private String getSimpleName( String name )
	{
		return name.substring( name.lastIndexOf( '.' )+1 );
	}

	/**
	 * @param walker called with each and every node under this one.
	 */
	@Override
	public void treewalk( TreeWalker walker ) throws ParseException
	{
		walker.preService( this );
		
		for (Named<?> n: this)
			n.treewalk( walker );
		
		walker.postService( this );
	}
}
