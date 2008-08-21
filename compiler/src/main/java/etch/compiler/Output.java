package etch.compiler;

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
}
