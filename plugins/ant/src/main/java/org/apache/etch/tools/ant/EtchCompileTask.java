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

package org.apache.etch.tools.ant;

import java.io.File;
import java.util.StringTokenizer;

import org.apache.etch.compiler.CmdLineOptions;
import org.apache.etch.compiler.EtchCompiler;
import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.DirectoryScanner;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.types.DirSet;


/**
 * Ant task for compiling Etch sources files.
 * 
 */
public class EtchCompileTask extends Task
{
	/**
	 * Creates a new ant task for running the Etch compiler.
	 */
	public EtchCompileTask()
	{
		// nothing to do.
	}

	private CmdLineOptions clo = new CmdLineOptions();

	/////////

	/**
	 * The list of paths to search for included or mixed in files (-I option).
	 *
	 * @param value
	 */
	public void addIncludes( DirSet value )
	{
		DirectoryScanner ds = value.getDirectoryScanner( getProject() );
		for (String dir : ds.getIncludedDirectories())
			clo.includePath.add( new File( ds.getBasedir(), dir ) );
	}

	/**
	 * The source file to compile.
	 *
	 * @param value
	 */
	public void setFile( File value )
	{
		if (clo.sourceFile != null)
			throw new BuildException( "only one source file at a time, please" );
		clo.sourceFile = value;
	}

	/**
	 * The destination directory of the generated files (-d option). If not
	 * specified, the same directory of the source file.
	 *
	 * @param value
	 */
	public void setOutputDir( File value )
	{
		clo.outputDir = value;
	}

	/**
	 * The binding name. Example values are java, csharp, python, ruby, c, and
	 * xml (-b option).
	 *
	 * @param value
	 */
	public void setBinding( String value )
	{
		clo.binding = value;
	}

	/**
	 * This specifies the file(s) we need to generate (-w option). Valid values
	 * depend upon the binding, but examples include BOTH, SERVER, CLIENT, ALL,
	 * INTF, IMPL, MAIN, NONE, and FORCE. HELP might give you some. Separate
	 * values using one or more characters from ",;: " (e.g., "BOTH, INTF",
	 * which is also the default for many bindings).
	 *
	 * @param value
	 */
	public void setWhat( String value )
	{
		StringTokenizer st = new StringTokenizer( value,
			CmdLineOptions.WHAT_DELIMETER );
		while (st.hasMoreElements())
			clo.what.add( st.nextToken().toUpperCase() );
	}

	/**
	 * Flag indicates whether to ignore the globally reserved word list (-g
	 * option).
	 *
	 * @param value
	 */
	public void setIgnoreGlobalWordsList( boolean value )
	{
		clo.ignoreGlobalWordsList = value;
	}

	/**
	 * Flag indicates whether to ignore the locally reserved word list (-l
	 * option).
	 *
	 * @param value
	 */
	public void setIgnoreLocalWordsList( boolean value )
	{
		clo.ignoreLocalWordsList = value;
	}

	/**
	 * The path of the user-defined reserved words list (-W option).
	 *
	 * @param value
	 */
	public void setUserWordsList( File value )
	{
		clo.userWordsList = value;
	}

	/**
	 * Ignore the ETCH_INCLUDE_PATH environment variable (-i option).
	 *
	 * @param value
	 */
	public void setIgnoreIncludePath( boolean value )
	{
		clo.ignoreIncludePath = value;
	}

	/**
	 * Flag indicates that mixin artifacts should not be generated (-n option).
	 * If false, mixin artifacts are generated into mixinOutputDir.
	 *
	 * @param value
	 */
	public void setNoMixinArtifacts( boolean value )
	{
		clo.noMixinArtifacts = value;
	}

	/**
	 * The destination directory of the generated mixin files (-m option). If
	 * not specified, and if noMixinArtifacts allows, mixin artifacts are
	 * generated into outputDir.
	 *
	 * @param value
	 */
	public void setMixinOutputDir( File value )
	{
		clo.mixinOutputDir = value;
	}

	/**
	 * Flag indicates whether the module name should should be flattened to
	 * produce a single directory or a directory tree (e.g., for csharp) (-f
	 * option).
	 *
	 * @param value
	 */
	public void setNoFlattenPackages( boolean value )
	{
		clo.noFlattenPackages = value;
	}

	/**
	 * Destination directory of the user editable template files (-t option).
	 * If not specified, same as outputDir.
	 *
	 * @param value
	 */
	public void setTemplateOutputDir( File value )
	{
		clo.templateOutputDir = value;
	}

	/**
	 * Flag indicates that the compiler should not report progress (-q option).
	 * 
	 * @param value
	 */
	public void setQuiet( boolean value )
	{
		clo.quiet = value;
	}

	/**
	 * We're just testing the compiler, don't write any files (--testing option).
	 * 
	 * @param value 
	 */
	public void setTesting( boolean value )
	{
		clo.testing = value;
	}

	/** 
	 * Sets the location of the etch installation (this is the directory which
	 * contains bin and lib) (no corresponding command line option).
	 *
	 * @param value
	 */
	public void setHome( File value )
	{
		home = value;
	}

	private File home;

	/** EXECUTION **/

	@Override
	/**
	 * execute
	 */
	public void execute() throws BuildException
	{
		ClassLoader cl;

		try
		{
			cl = EtchCompiler.setupClassLoader( home );
		}
		catch ( Exception e )
		{
			e.printStackTrace();
			throw new BuildException(
				"problem setting up etch compiler class loader", e );
		}

		EtchCompiler etchCompiler;

		try
		{
			// Instantiate a new compiler instance
			etchCompiler = new EtchCompiler( cl );
		}
		catch ( Exception e )
		{
			e.printStackTrace();
			throw new BuildException( "problem setting up etch compiler", e );
		}

		try
		{
			etchCompiler.run( clo );
		}
		catch ( Exception e )
		{
			e.printStackTrace();
			throw new BuildException( "problem running etch compiler", e );
		}
		
		if (clo.lh.hasError())
			throw new BuildException( "problem running etch compiler" );
	}
}
