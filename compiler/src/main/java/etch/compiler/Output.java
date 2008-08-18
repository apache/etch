package etch.compiler;

import java.io.OutputStream;

public interface Output
{
	public OutputStream newOutputStream( String fn );

	public Output newPackage( String pn );
	
	public void report();
}
