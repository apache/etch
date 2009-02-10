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

package org.apache.etch.compiler;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.net.URI;
import java.net.URL;

import org.junit.Test;

/**
 * Test of EtchMain.
 */
public class TestEtchMain
{
	private final MyEtchMain prog = new MyEtchMain();
	
	private final static String T = "--testing";
	
	private final static String BINDING = "null";
	
	/** @throws Exception */
	@Test
	public void test1() throws Exception
	{
		// option -b not specified
		prog.doMain( new String[] { T } );
		assertEquals( 1, prog.code );
	}
	
	/** @throws Exception */
	@Test
	public void test2() throws Exception
	{
		// missing value for option -b
		prog.doMain( new String[] { T, "-b" } );
		assertEquals( 1, prog.code );
	}
	
	/** @throws Exception */
	@Test
	public void test3() throws Exception
	{
		// missing file name
		prog.doMain( new String[] { T, "-b", "xxx" } );
		assertEquals( 1, prog.code );
	}
	
	/** @throws Exception */
	@Test
	public void test4() throws Exception
	{
		// binding name is bogus
		prog.doMain( new String[] { T, "-b", "xxx", "yyy" } );
		assertEquals( 3, prog.code );
	}
	
	/** @throws Exception */
	@Test
	public void test5() throws Exception
	{
		// source file doesn't exist
		prog.doMain( new String[] { T, "-b", BINDING, "yyy" } );
		assertEquals( 3, prog.code );
	}
	
	/** @throws Exception */
	@Test
	public void test6() throws Exception
	{
		// source file doesn't end in .etch
		prog.doMain( new String[] { T, "-b", BINDING, findTestTxt() } );
		assertEquals( 3, prog.code );
	}

	/** @throws Exception */
	@Test
	public void test7() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "-b", BINDING, findTestEtch() } );
		if (prog.code != 0)
			System.out.println( "TestEtchMain.test7 msgs = "+
				((ListLogHandler) prog.testingClo.lh).msgs );
		assertEquals( 0, prog.code );
	}

	/** @throws Exception */
	@Test
	public void test8() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] {} );
		assertNull( prog.testingClo );
	}

	/** @throws Exception */
	@Test
	public void test9() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test10() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "-q" } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertTrue( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test11() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "--quiet" } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertTrue( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test12() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "-g" } );
		assertTrue( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test13() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "--ignore-global-words-list" } );
		assertTrue( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test14() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "-l" } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertTrue( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test15() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "--ignore-local-words-list" } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertTrue( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test16() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "-i" } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertTrue( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test17() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "--ignore-include-path" } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertTrue( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test18() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "-n" } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertTrue( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test19() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "--no-mixin-artifacts" } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertTrue( prog.testingClo.noMixinArtifacts );
		assertFalse( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test20() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "-f" } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertTrue( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}

	/** @throws Exception */
	@Test
	public void test21() throws Exception
	{
		// compilation should have worked.
		prog.doMain( new String[] { T, "--no-flatten-packages" } );
		assertFalse( prog.testingClo.ignoreGlobalWordsList );
		assertFalse( prog.testingClo.ignoreLocalWordsList );
		assertFalse( prog.testingClo.noMixinArtifacts );
		assertTrue( prog.testingClo.noFlattenPackages );
		assertFalse( prog.testingClo.quiet );
		assertFalse( prog.testingClo.ignoreIncludePath );
		assertTrue( prog.testingClo.testing );
	}
	
	private String findTestTxt() throws Exception
	{
		return findFile( "test.txt" );
	}
	
	private String findTestEtch() throws Exception
	{
		return findFile( "test.etch" );
	}
	
	private String findFile( String fn ) throws Exception
	{
		URL u = getClass().getResource( fn );
		File f = new File( new URI( u.toString() ) );
		return f.getAbsolutePath();
	}

	private static class MyEtchMain extends EtchMain
	{
		@Override
		public void exit( int code, String source, String reason,
			boolean emergency )
		{
			this.code = code;
		}
		
		/** the exit code */
		public int code;
	}
}
