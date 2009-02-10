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

import java.io.File;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

/**
 * This class defines options to be passed to the Etch compiler.
 */
public class CmdLineOptions
{
	/**
	 * Name of the environment variable which contributes to the include path.
	 */
	public final static String ETCH_INCLUDE_PATH = "ETCH_INCLUDE_PATH";
	
    /**
     * Separators for items in the "what" list.
     */
    public static String WHAT_DELIMETER = ",;: ";
	
	/**
	 * Constructs a new CmdLineOptions from an existing one.
	 * @param other
	 */
	public CmdLineOptions( CmdLineOptions other )
	{
		includePath = new ArrayList<File>( other.includePath );
		ignoreGlobalWordsList = other.ignoreGlobalWordsList;
		ignoreLocalWordsList = other.ignoreLocalWordsList;
		userWordsList = other.userWordsList;
		outputDir = other.outputDir;
		binding = other.binding;
		what = new HashSet<String>( other.what );
		mixinOutputDir = other.mixinOutputDir;
		noMixinArtifacts = other.noMixinArtifacts;
		noFlattenPackages = other.noFlattenPackages;
		quiet = other.quiet;
		templateOutputDir = other.templateOutputDir;
		ignoreIncludePath = other.ignoreIncludePath;
		testing = other.testing;
		sourceFile = other.sourceFile;
		// EPHEMERAL STUFF //
		cl = other.cl;
		lh = other.lh;
		effectiveIncludePath = new ArrayList<File>( other.effectiveIncludePath );
		output = other.output;
		templateOutput = other.templateOutput;
		mixinOutput = other.mixinOutput;
		bindingClass = other.bindingClass;
		backend = other.backend;
	}
	
	/**
	 * Constructs a new CmdLineOptions.
	 */
	public CmdLineOptions()
	{
		includePath = new ArrayList<File>();
		what = new HashSet<String>();
		effectiveIncludePath = new ArrayList<File>();
	}
	
	/**
	 * The list of paths to search for included or mixed in files (-I option).
	 */
	public List<File> includePath;

	/**
	 * Flag indicates whether to ignore the globally reserved word list (-g
	 * option).
	 */
	public boolean ignoreGlobalWordsList;

	/**
	 * Flag indicates whether to ignore the locally reserved word list (-l
	 * option).
	 */
	public boolean ignoreLocalWordsList;

	/**
	 * The path of the user-defined reserved words list (-W option).
	 */
	public File userWordsList;

	/**
	 * The destination directory of the generated files (-d option). If not
	 * specified, the same directory of the source file.
	 */
	public File outputDir;

	/**
	 * The binding name. Example values are java, csharp, python, ruby, c, and
	 * xml (-b option).
	 */
	public String binding;

	/**
	 * This specifies the file(s) we need to generate (-w option). Valid values
	 * depend upon the binding, but examples include BOTH, SERVER, CLIENT, ALL,
	 * INTF, IMPL, MAIN, NONE, and FORCE. HELP might give you some. Separate
	 * values using one or more characters from ",;: " (e.g., "BOTH, INTF",
	 * which is also the default for many bindings).
	 */
	public Set<String> what;

	/**
	 * The destination directory of the generated mixin files (-m option). If
	 * not specified, and if {@link #noMixinArtifacts} allows, mixin artifacts
	 * are generated into {@link #outputDir}.
	 */
	public File mixinOutputDir;

	/**
	 * Flag indicates that mixin artifacts should not be generated (-n option).
	 * If false, mixin artifacts are generated into {@link #mixinOutputDir}.
	 */
	public boolean noMixinArtifacts;

	/**
	 * Flag indicates whether the module name should should be flattened to
	 * produce a single directory or a directory tree (e.g., for csharp) (-f
	 * option).
	 */
	public boolean noFlattenPackages;

	/**
	 * Flag indicates that the compiler should not report progress (-q option).
	 */
	public boolean quiet;

	/**
	 * Destination directory of the user editable template files (-t option).
	 * If not specified, same as {@link #outputDir}.
	 */
	public File templateOutputDir;

	/**
	 * Ignore the ETCH_INCLUDE_PATH environment variable (-i option).
	 */
	public boolean ignoreIncludePath;

	/**
	 * We're just testing the compiler, don't write any files (--testing option).
	 */
	public boolean testing;

	/**
	 * The source file to compile.
	 */
	public File sourceFile;

	/////////////////////
	// EPHEMERAL STUFF //
	/////////////////////

	/**
	 * The binding class loader.
	 */
	public ClassLoader cl;
	
	/**
	 * Destination of various error or status messages.
	 */
	public LogHandler lh;

	/**
	 * Computed effective includePath, including parent dir of source file,
	 * include path command line elements, and path elements from system
	 * environment variable.
	 */
	public List<File> effectiveIncludePath;

	/**
	 * Output for service generated files.
	 */
	public Output output;

	/**
	 * Output for service user-editable generated files.
	 */
	public Output templateOutput;

	/**
	 * Output for mixed in artifacts.
	 */
	public Output mixinOutput;
    
	/**
	 * The class which implements the binding.
	 */
	public Class<?> bindingClass;
    
	/**
	 * The instance of bindingClass used to generated code.
	 */
	public Backend backend;
}
