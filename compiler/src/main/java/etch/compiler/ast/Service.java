/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.compiler.ast;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;

import etch.compiler.Backend;
import etch.compiler.CmdLineOptions;
import etch.compiler.Etch2;
import etch.compiler.EtchGrammarConstants;
import etch.compiler.LogHandler;
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
		
		try {
		

			getCmdLineOptions().lh.logMessage( LogHandler.LEVEL_INFO, n.token,
			" Checking Mixin file: " + n.name + " at line : "
				+ n.token.beginLine + "\n" );
		String filename = n.name;
		filename = filename.replace( '.', '/' );
		filename = filename + ".etch";
		List<File> list = getCmdLineOptions().includePath;
		String searchPath = null;
		File etchFile = null;
		boolean mixinFileExist = false;
		
		
		if (list != null)
		{
			for (int i = 0; i < list.size(); i++)
			{
				File f = list.get( i );
				if (f != null)
				{
					searchPath = f.getAbsolutePath() + "/" + filename;
					etchFile = new File( searchPath );
					if (etchFile.exists())
					{
						mixinFileExist = true;
						break;
					}
				}
			}
			if (mixinFileExist)
			{

				getCmdLineOptions().lh.logMessage( LogHandler.LEVEL_INFO, n.token,
					" Found mixin file " + etchFile.getAbsolutePath()
						+ " in Include Path \n" );
				// System.out.println(" We found the file " + etchFile.getName()
				// + "
				// Now lets try to generate the parsed tree");
				
				// Does -d or -dm option exist. If not return
				if (!getCmdLineOptions().noMixinArtifacts)
				{
					if (getCmdLineOptions().outputDir == null
						&& getCmdLineOptions().mixinOutputDir == null)
					{
						getCmdLineOptions().lh
						.logMessage(
							LogHandler.LEVEL_ERROR,
							n.token,
							"-d or -m option is not specified. Please specify one of these"
							+ " options. Mixin artifacts will only be generated when atleast one of these options"
							+ " is present \n" );

						return null;
					}
				}
				
				InputStream is = null;
				try
				{
					is = new java.io.FileInputStream( etchFile );
				}
				catch ( FileNotFoundException e1 )
				{
					e1.printStackTrace();

				}

				getCmdLineOptions().lh.logMessage( LogHandler.LEVEL_INFO, null,
					"Parsing file " + etchFile.getAbsolutePath() + "\n" );
				getCmdLineOptions().lh.push( etchFile.getName(), new Integer(
					n.token.beginLine ) );
				
				CmdLineOptions cmdLineObject = new CmdLineOptions(getCmdLineOptions());
				HashSet<String> set = new HashSet<String>();
				if (getCmdLineOptions().noMixinArtifacts)
				{
					cmdLineObject.isMixinPresent = false;
					set.add(Backend.WHAT_NONE);
				}
				else
				{
					cmdLineObject.isMixinPresent = true;
					set = populateWhat(getCmdLineOptions().what);
				}
				cmdLineObject.what = set;
				
				
				Module retMod = Etch2.doCompile( cmdLineObject, is,
					getCmdLineOptions().lh );
				getCmdLineOptions().lh.pop();
				Mixin mixin = new Mixin( this, n, nOpts, retMod );
				nameList.add( n, mixin );
				return mixin;
			}
			getCmdLineOptions().lh.logMessage( LogHandler.LEVEL_ERROR, n.token,
				" Mixin file does not exist in Include Path. \n" );
		}
		else
		{
			getCmdLineOptions().lh.logMessage( LogHandler.LEVEL_ERROR, n.token,
				" No Include Path defined for Mixin File \n" );
			
		}
		}
		catch (Exception e) {
			getCmdLineOptions().lh.logMessage( LogHandler.LEVEL_ERROR, n.token,
			" Unexpected Error occured during parsing mixin \n" );
			e.printStackTrace();
			
		}

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
		TypeRef rmTypeRef = new TypeRef( rmType );
		
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
			return (Named<?>)list.get( 0 );


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
	
	private HashSet<String> populateWhat(Set<String> set) {
		HashSet<String> retSet = new HashSet<String>();
		if (set.contains( Backend.WHAT_ALL ) || set.contains( Backend.WHAT_BOTH )) {
			retSet.add( Backend.WHAT_BOTH );
		}
		if (set.contains( Backend.WHAT_CLIENT)) {
			retSet.add( Backend.WHAT_CLIENT );
		}
		if (set.contains( Backend.WHAT_SERVER)) {
			retSet.add( Backend.WHAT_SERVER );
		}
		if (set.contains( Backend.WHAT_MAIN ) || set.contains( Backend.WHAT_IMPL ) ||
			set.contains( Backend.WHAT_HELPER )) {
			if (retSet.isEmpty()) {
				retSet.add( Backend.WHAT_BOTH );
			}
		}
		if (set.contains( Backend.WHAT_FORCE ) ) {
			
				retSet.add( Backend.WHAT_FORCE );
			
		}
		if (set.contains( Backend.WHAT_NONE)) {
			retSet.clear();
			retSet.add( Backend.WHAT_NONE );
		}

		return retSet;
		
	}
}
