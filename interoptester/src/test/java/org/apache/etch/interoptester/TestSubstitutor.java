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


import org.apache.etch.interoptester.Substitutor;
import org.junit.Test;

import static org.junit.Assert.*;

/** */
public class TestSubstitutor
{
	private final Map<String, String> map = new HashMap<String, String>();
	{
		map.put( "a", "1" );
		map.put( "b", "2" );
	}
	
	/** @throws Exception */
	@Test
	public void subst1() throws Exception
	{
		assertEquals( "", Substitutor.subst( "", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst2() throws Exception
	{
		assertEquals( "a", Substitutor.subst( "a", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst3() throws Exception
	{
		assertEquals( "1", Substitutor.subst( "{a}", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst4() throws Exception
	{
		assertEquals( "x2y", Substitutor.subst( "x{b}y", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst5() throws Exception
	{
		assertEquals( "{c}", Substitutor.subst( "{c}", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst5a() throws Exception
	{
		assertEquals( "{{c}}", Substitutor.subst( "{{c}}", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst6() throws Exception
	{
		assertEquals( "{1}", Substitutor.subst( "{{a}}", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst7() throws Exception
	{
		assertEquals( "{a", Substitutor.subst( "{a", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst7a() throws Exception
	{
		assertEquals( "a}", Substitutor.subst( "a}", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst8() throws Exception
	{
		assertEquals( "{a}", Substitutor.subst( "\\{a}", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst9() throws Exception
	{
		assertEquals( "a", Substitutor.subst( "\\a", map ) );
	}
	
	/** @throws Exception */
	@Test
	public void subst10() throws Exception
	{
		assertEquals( "\\", Substitutor.subst( "\\\\", map ) );
	}
}
