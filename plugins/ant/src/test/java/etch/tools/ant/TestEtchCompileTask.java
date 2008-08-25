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

package etch.tools.ant;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.FilenameFilter;
import java.util.ArrayList;
import java.util.List;

import org.apache.tools.ant.Project;
import org.apache.tools.ant.types.DirSet;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

/**
 * Test of EtchCompileTask
 * TODO EtchCompileTask needs to be rewritten in the proper style (e.g., TestEtchMain).
 */
public class TestEtchCompileTask
{

	private static File outputDir;

	/** @throws Exception */
	//@BeforeClass
	public static void setUp() throws Exception
	{
		outputDir = new File( TestEtchCompileTask.class.getResource(
			"/MediaEngines.txt" ).getFile() ).getParentFile();
	}

	/** @throws Exception */
	@Test @Ignore
	public void testCompileJava() throws Exception
	{
		File outdir = new File( outputDir, "java" );
		outdir.delete();
		outdir.mkdir();

		EtchCompileTask task = new EtchCompileTask();
		task.setProject( buildProject() );
		task.setBinding( "java" );
		task.setFile( new File( TestEtchCompileTask.class.getResource(
			"Management.etch" ).getFile() ) );
		// TODO handle exception better
		try
		{
		    task.setOutputDir( outdir );
		}
		catch (Exception e)
		{
		    assertTrue(false);
		}
		for (DirSet ds : buildIncludes())
			task.addIncludes( ds );
		task.execute();

		// test for existence of three output files
		assertTrue( containsFile( outdir, "Management.java" ) );
		assertTrue( containsFile( outdir, "StubManagement.java" ) );
		assertTrue( containsFile( outdir, "RemoteManagement.java" ) );
		// sanity check
		assertFalse( containsFile( outdir, "ValueFactoryManagement.cs" ) );
	}

	/** @throws Exception */
	@Test @Ignore
	public void testCompileCSharp() throws Exception
	{
		File outdir = new File( outputDir, "csharp" );
		outdir.delete();
		outdir.mkdir();

		EtchCompileTask task = new EtchCompileTask();
		task.setProject( buildProject() );
		task.setBinding( "csharp" );
		task.setFile( new File( TestEtchCompileTask.class.getResource(
			"Management.etch" ).getFile() ) );
        // TODO handle exception better
		try
		{
		    task.setOutputDir( outdir );
		}
		catch (Exception e)
		{
		    assertTrue(false);
		}
		for (DirSet ds : buildIncludes())
			task.addIncludes( ds );
		task.execute();

		assertTrue( containsFile( outdir, "ValueFactoryManagement.cs" ) );
		// sanity check
		assertFalse( containsFile( outdir, "Management.java" ) );
	}

	/** @throws Exception */
	@Test @Ignore
	public void testCompileXml() throws Exception
	{
		File outdir = new File( outputDir, "xml" );
		outdir.delete();
		outdir.mkdir();

		EtchCompileTask task = new EtchCompileTask();
		task.setProject( buildProject() );
		task.setBinding( "xml" );
		task.setFile( new File( TestEtchCompileTask.class.getResource(
			"Management.etch" ).getFile() ) );
        // TODO handle exception better
		try
		{
		    task.setOutputDir( outdir );
		}
		catch (Exception e)
		{
		    assertTrue(false);
		}
		for (DirSet ds : buildIncludes())
			task.addIncludes( ds );
		task.execute();

		assertTrue( containsFile( outdir, "Management.xml" ) );
		// sanity check
		assertFalse( containsFile( outdir, "Management.java" ) );
	}

	/** @throws Exception */
	@Test @Ignore
	public void testCompileMixin() throws Exception
	{
		File outdir = new File( outputDir, "mixin" );
		outdir.delete();
		outdir.mkdir();

		EtchCompileTask task = new EtchCompileTask();
		task.setProject( buildProject() );
		task.setBinding( "java" );
		task.setFile( new File( TestEtchCompileTask.class.getResource(
			"/etch/examples/mixin/Foo.etch" ).getFile() ) );
		try
		{
		    task.setOutputDir( outdir );
		}
		catch ( Exception e )
		{
		    assertTrue(false);
		}
		for (DirSet ds : buildIncludes())
			task.addIncludes( ds );
		task.execute();

		assertTrue( containsFile( outdir, "Foo.java" ) );
		assertTrue( containsFile( outdir, "Bar.java" ) );
		assertTrue( containsFile( outdir, "Baz.java" ) );
//		// sanity check
	}

	private List<DirSet> buildIncludes()
	{
		List<DirSet> dirs = new ArrayList<DirSet>();

		DirSet ds = new DirSet();
		ds.setDir( new File( TestEtchCompileTask.class.getResource(
			"/MediaEngines.txt" ).getFile() ).getParentFile() );
		dirs.add( ds );

		ds = new DirSet();
		ds.setDir( new File( TestEtchCompileTask.class.getResource(
			"/etch/examples/mixin/bar" ).getFile() ) );
		dirs.add( ds );

		ds = new DirSet();
		ds.setDir( new File( TestEtchCompileTask.class.getResource(
			"/etch/examples/mixin/baz" ).getFile() ) );
		dirs.add( ds );

		return dirs;
	}

	private Project buildProject()
	{
		Project p = new Project();
		return p;
	}

	private boolean containsFile( File dir, final String filename )
	{
		FilenameFilter filter = new FilenameFilter()
		{
			public boolean accept( File dir, String name )
			{
				return filename.equals( name );
			}
		};
		boolean found = false;
		if (dir.list( filter ).length > 0)
		{
			found = true;
		}
		else
		{
			for (File f : dir.listFiles())
			{
				if (f.isDirectory())
				{
					if (found = containsFile( f, filename ))
						break;
				}
			}
		}
		return found;
	}
}
