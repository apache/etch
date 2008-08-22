package etch.compiler;

import java.io.File;
import java.io.IOException;
import java.io.OutputStream;

/**
 * Interface of an output container for the compiler.
 */
public interface Output
{
	/**
	 * Creates an OutputStream for the output file.
	 * @param fn
	 * @return a new OutputStream for the output file.
	 */
	public OutputStream newOutputStream( String fn );

	/**
	 * Creates a new Output to hold the output for the package.
	 * @param pn
	 * @return a new Output.
	 */
	public Output newPackage( String pn );
	
	/**
	 * Reports on the contents of the Output.
	 * @param what
	 */
	public void report( String what );

	/**
	 * Saves the files in this output container.
	 * @param outputDir the directory for the output files.
	 * @param noQualOutputDir if true do not qualify the file name with the
	 * module name.
	 * @param noFlattenPackages if true do not flatten the directory structure
	 * implied by the module name.
	 * @param force if true always overwrite files.
	 * @throws IOException 
	 */
	public void saveFiles( File outputDir, boolean noQualOutputDir,
		boolean noFlattenPackages, boolean force ) throws IOException;
}
