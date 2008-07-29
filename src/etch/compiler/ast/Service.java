/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import etch.compiler.Backend;
import etch.compiler.EtchGrammarConstants;
import etch.compiler.ParseException;
import etch.compiler.Token;

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
	
	@Override
	public Integer id()
	{
		return null;
	}

	/**
	 * Adds a mixin declaration.
	 * @param n
	 * @param nOpts 
	 * @return the added mixin.
	 */
	public Mixin addMixin( Name n, Map<String, Opt> nOpts )
	{
		// TODO Auto-generated method stub
		return null;
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
		TypeRef rmTypeRef = new TypeRef( rmType );
		
		nameList.check( rmName );
		Message rm = new Message( this, rmName, new HashMap<String, Opt>(),
			rmTypeRef );
		rm.setMessageDirection( getResultMessageDirection( reqMsg.msgDir() ) );
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
		return nameList.get( name );
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
