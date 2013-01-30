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

import java.util.Map;
import java.util.StringTokenizer;

import org.apache.etch.compiler.LogHandler;
import org.apache.etch.compiler.Token;


/**
 * Checks the abstract syntax tree for uses of reserved words.
 */
public class ReservedWordChecker extends AbstractTreeWalker
{
	/**
	 * Constructs the reserved word checker with the word list.
	 * @param words the reserved word list including translations.
	 * @param rewrite flag which indicates we should attempt to rewrite
	 * the word.
	 * @param lh 
	 */
	public ReservedWordChecker( Map<String, String> words, boolean rewrite, LogHandler lh )
	{
		if (rewrite)
			throw new IllegalArgumentException( "rewrite not supported yet" );
		
		this.words = words;
		this.rewrite = rewrite;
		this.lh = lh;
	}
	
	private final Map<String, String> words;
	
	private final boolean rewrite;
	
	private LogHandler lh;

	/**
	 * @return true if the tree walk was ok, false if there
	 * were collisions which were not edited away.
	 */
	public boolean ok()
	{
		return ok;
	}
	
	private boolean ok = true;
	
	@Override
	public void preModule( Module module )
	{
		Name name = module.name();
		StringTokenizer st = new StringTokenizer( name.name, "." );
		while (st.hasMoreTokens())
		{
			String s = st.nextToken();
			if (words.containsKey( s ))
			{
				whine( "module name component", name.token );
				break;
			}
		}
	}
	
	@Override
	public void preService( Service service )
	{
		Name name = service.name();
		if (words.containsKey( name.name ))
			whine( "service name", name.token );
	}
	
	@Override
	public void preConstant( Constant constant )
	{
		Name name = constant.name();
		if (words.containsKey( name.name ))
			whine( "constant name", name.token );
	}
	
	@Override
	public void preEnum( Enumx enumx )
	{
		Name name = enumx.name();
		if (words.containsKey( name.name ))
			whine( "enum name", name.token );
	}
	
	@Override
	public void doItem( Item item )
	{
		Name name = item.name();
		if (words.containsKey( name.name ))
			whine( "enum item name", name.token );
	}
	
	@Override
	public void preStruct( Struct struct )
	{
		Name name = struct.name();
		if (words.containsKey( name.name ))
			whine( "struct name", name.token );
	}
	
	@Override
	public void doExtern( Extern extern )
	{
		Name name = extern.name();
		if (words.containsKey( name.name ))
			whine( "extern name", name.token );
	}
	
	@Override
	public void preExcept( Except except )
	{
		Name name = except.name();
		if (words.containsKey( name.name ))
			whine( "exception name", name.token );
	}
	
	@Override
	public void preMessage( Message message )
	{
		Name name = message.name();
		if (words.containsKey( name.name ))
			whine( "message name", name.token );
	}
	
	@Override
	public void preParameter( Parameter parameter )
	{
		Name name = parameter.name();
		if (words.containsKey( name.name ))
			whine( "parameter name", name.token );
	}

	private void whine( String what, Token token )
	{
		String msg = String.format( "Etch: %s '%s' is a reserved word at line %d\n", what, token.image, token.beginLine);
		if (rewrite)
			lh.report( LogHandler.LEVEL_WARNING, token.beginLine, msg );
		else
			lh.report( LogHandler.LEVEL_ERROR, token.beginLine, msg );
		ok = rewrite;
	}
}
