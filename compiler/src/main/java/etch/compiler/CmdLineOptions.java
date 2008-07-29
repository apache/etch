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
 * This class contains options to be passed to the Etch compiler.
 */
public class CmdLineOptions
{
	/**
	 * Constructs a new CmdLineOptions from an existing one.
	 * @param other
	 */
	public CmdLineOptions( CmdLineOptions other )
	{
		includePath = new ArrayList<File>(other.includePath);
		ignoreGlobal = other.ignoreGlobal;
		ignoreLocal = other.ignoreLocal;
		userWordsList = other.userWordsList;
		outputDir = other.outputDir;
		binding = other.binding;
		what = new HashSet<String>(other.what);
		bindingClass = other.bindingClass;
		mixinOutputDir = other.mixinOutputDir;
		noMixinArtifacts = other.noMixinArtifacts;
		isMixinPresent = other.isMixinPresent;
		noFlattenPackages = other.noFlattenPackages;
		quiet = other.quiet;
		lh = other.lh;
	}
	
	/**
	 * Constructs a new CmdLineOptions.
	 */
	public CmdLineOptions()
	{
		// nothing to do.
	}
	
	/**
	 * The list of paths to search for included or mixed in files (-I option).
	 */
	public List<File> includePath;

	/**
	 * Flag indicates whether to ignore the globally reserved word list (-g
	 * option).
	 */
	public boolean ignoreGlobal;

	/**
	 * Flag indicates whether to ignore the locally reserved word list (-l
	 * option).
	 */
	public boolean ignoreLocal;

	/**
	 * The path of the user-defined reserved words list (-W option).
	 */
	public String userWordsList;

	/**
	 * The location of the generated files (-d option).
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
	 * IMPL, MAIN, and NONE.
	 */
	public Set<String> what;

	/**
	 * This class specifies the binding to use. If the string binding is
	 * specified then this variable does not need to be defined. It is currently
	 * only used internally by Etch2.
	 * @deprecated the compiler should really specify binding name.
	 */
	@Deprecated
	public Class<?> bindingClass;

	/**
	 * The location of the generated mixin files (-m option).
	 */
	public File mixinOutputDir;

	/**
	 * Flag indicates that mixin artifacts should not be generated (-n option).
	 */
	public boolean noMixinArtifacts;

	/**
	 * Flag indicates that a mixin has been detected.
	 */
	public boolean isMixinPresent;

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
	 * Destination of various error or status messages.
	 */
	public LogHandler lh;
}
