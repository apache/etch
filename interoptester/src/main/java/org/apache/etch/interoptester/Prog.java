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

package org.apache.etch.interoptester;

import java.util.HashMap;
import java.util.Map;

import org.apache.etch.util.Assertion;
import org.apache.etch.util.core.xml.XmlParser.TagElement;


/**
 * Prog models a reference to a Program in a Test.
 */
public class Prog
	{
		/**
		 * @param test
		 * @param r
		 * @return the parsed Prog.
		 */
		public static Prog parse( TestIntf test, TagElement r )
		{
			Assertion.check( r.matches( null, "prog" ), "tag is prog" );
			
			String name = r.getAttr( null, "name" );
			if (name == null || name.length() == 0)
				throw new IllegalArgumentException( "prog tag has no name attribute" );
			
			Prog p = new Prog( test, name );
			p.parseBody( r );
			return p;
		}
		
		private Prog( TestIntf test, String name )
		{
			this.test = test;
			this.name = name;
		}

		private final TestIntf test;
		
		private final String name;

		/**
		 * @return the enclosing Test
		 */
		public TestIntf test()
		{
			return test;
		}

		/**
		 * @return the name of the Program to run.
		 */
		public String name()
		{
			return name;
		}
		
		private void parseBody( TagElement r )
		{
			new ChildWalker()
			{
				public boolean tag( TagElement te )
				{
					if (te.matches( null, "arg" ))
					{
						Arg a = Arg.parse( te );
						if (args.containsKey( a.name() ))
							throw new IllegalArgumentException( "duplicate name arg "+a.name() );
						args.put( a.name(), a );
						return true;
					}
					
					return false;
				}
			}.walk( r );
		}
		
		private final Map<String, Arg> args = new HashMap<String, Arg>();

		/**
		 * @return the args to be passed to this prog.
		 */
		public Map<String, Arg> args()
		{
			return args;
		}

		/**
		 * @param substs
		 * @return a running instance of Prog.
		 * @throws Exception 
		 */
		public RunningProg start( Map<String, String> substs ) throws Exception
		{
			Map<String, String> overrides = processSubst( "start", substs );
			return getProgram().start( overrides );
		}

		/**
		 * @param substs
		 * @throws Exception 
		 */
		public void run( Map<String, String> substs ) throws Exception
		{
			Map<String, String> overrides = processSubst( "run", substs );
			getProgram().run( overrides );
		}

		private Map<String, String> processSubst( String who,
			Map<String, String> substs )
		{
			//System.out.println( "Prog."+who+": name "+name+", substs = "+substs );
			
			Map<String, String> overrides = Arg.processSubst( "prog", args,
				substs );
			
			//System.out.println( "Prog."+who+": name "+name+", overrides = "+overrides );
			
			return overrides;
		}

		private Program getProgram()
		{
			Program p = test.getProgram( name );
			if (p == null)
				throw new IllegalArgumentException( "no such program "+name );
			return p;
		}
	}