package etch.compiler;

import java.io.File;
import java.util.List;
import java.util.Set;

/**
 * This class contains Command Line Options to be passed to the Etch
 * Compiler
 */

public class CmdLineOptions
{
	/** This list contains the files included in the etch file **/
	public static List<File> includePath;

	/** This boolean value indicates whether to ignore the globally reserved word list */
	public boolean ignoreGlobal;
	/** This boolean value indicates whether to ignore the locally reserved word list */
	public boolean ignoreLocal;
	/** This string contains the user-defined reserved words list **/
	public String userWordsList;
	/** The location where the files will be generated **/
	public File outputDir;
	/** This specifies the binding. Valid values are java,csharp,ruby and xml **/
	public String binding;
	/** This specifies the file we need to generate. Valid values are ALL, BOTH,
	 * SERVER, CLIENT, IMPL, MAIN, HELPER, NONE
	 */
	public Set<String> what;

	/** This class specifies the binding to use. If the string binding is specified then this
	 * variable does not need to be defined. It is currently only used internally by Etch2
	 */
	public Class<?> bindingClass;
	/** The location where the mixin files will be generated **/
	public File mixinOutputDir;
	/** Boolean Value Specifying whether generation of mixin artifacts is suppressed or not **/
	public boolean isMixinSuppressed;
	
	public boolean isMixinPresent = false;
	
	/** Specifies if the package tree should be flattened (applicable to csharp) */
	public boolean flattenPackages = true;

}
