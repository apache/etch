/*
 * $Id$
 * 
 * Copyright 2007-2008 Cisco Systems Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package etch.compiler;

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
		includePath = new ArrayList<File>(other.includePath);
		ignoreGlobalWordsList = other.ignoreGlobalWordsList;
		ignoreLocalWordsList = other.ignoreLocalWordsList;
		userWordsList = other.userWordsList;
		outputDir = other.outputDir;
		binding = other.binding;
		what = new HashSet<String>(other.what);
		mixinOutputDir = other.mixinOutputDir;
		noMixinArtifacts = other.noMixinArtifacts;
		noFlattenPackages = other.noFlattenPackages;
		quiet = other.quiet;
		templateOutputDir = other.templateOutputDir;
		ignoreIncludePath = other.ignoreIncludePath;
		sourceFile = other.sourceFile;
		// EPHEMERAL STUFF //
		cl = other.cl;
		lh = other.lh;
		isMixinPresent = other.isMixinPresent;
		effectiveIncludePath = other.effectiveIncludePath;
		noDirOnOutputFiles = other.noDirOnOutputFiles;
		workingDir = other.workingDir;
		output = other.output;
		templateOutput = other.templateOutput;
		mixinOutput = other.mixinOutput;
	}
	
	/**
	 * Constructs a new CmdLineOptions.
	 */
	public CmdLineOptions()
	{
		includePath = new ArrayList<File>();
		what = new HashSet<String>();
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
	 * specified, the same directory of the source file. Not specifying also
	 * turns on {@link #noDirOnOutputFiles}.
	 */
	public File outputDir;

	/**
	 * The binding name. Example values are java, csharp, python, ruby, c, and
	 * xml (-b option).
	 */
	public String binding;

	/**
	 * This specifies the file(s) we need to generate (-w option). Valid values
	 * depend upon the binding, but examples include ALL, BOTH, SERVER, CLIENT,
	 * IMPL, MAIN, and NONE. HELP might give you some.
	 */
	public Set<String> what;

	/**
	 * The destination directory of the generated mixin files (-m option). If
	 * not specifed, and if {@link #noMixinArtifacts} allows, mixin artifacts
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
	 * Destination directory of the user editable template files. If not
	 * specified, same as {@link #outputDir}.
	 */
	public File templateOutputDir;

	/**
	 * Ignore the ETCH_INCLUDE_PATH environment variable.
	 */
	public boolean ignoreIncludePath;

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
	 * Flag indicates that mixin artifacts have been generated.
	 */
	public boolean isMixinPresent;

	/**
	 * Computed effective includePath, including parent dir of source file,
	 * include path command line elements, and path elements from system
	 * environment variable.
	 */
	public List<File> effectiveIncludePath;
	
	/**
	 * If true, output files are not qualified with a directory name derived
	 * from the module name. If false, output files are qualified with a
	 * directory name under control of {@link #noFlattenPackages}.
	 */
	public boolean noDirOnOutputFiles;

	/**
	 * The parent directory of the source. This is where we search for include
	 * files and also where we put output files if outputDir is not specified.
	 */
	public File workingDir;

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
}
