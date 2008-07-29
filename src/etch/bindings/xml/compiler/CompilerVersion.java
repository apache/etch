package etch.bindings.xml.compiler;

/**
 * The version info of this Etch backend (compiler).
 */
public interface CompilerVersion
{
	/** The default version of this Etch backend (compiler) */
	public String DEFAULT_COMPILER_VERSION = "java @EtchVersion@ (@EtchBuildTag@)";
	
	// This file is edited by the production build system to
	// replace DEFAULT_COMPILER_VERSION below with whatever
	// it wants the version string to actually be.
	
	/** The version of this Etch backend (compiler) */
	public String VERSION = DEFAULT_COMPILER_VERSION;
}
