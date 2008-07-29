package etch.examples.bigidl;

import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

/**
 * Program to construct a large idl file for testing limits.
 */
public class MakeBigIdl
{
	/**
	 * @param args
	 */
	public static void main( String[] args ) throws IOException
	{
		int n = 1000;
		PrintWriter pw = new PrintWriter( new FileWriter( "src/etch/examples/bigidl/BigIdl.etch" ) );
		pw.println( "module etch.examples.bigidl" );
		pw.println( "service BigIdl" );
		pw.println( "{" );
		for (int i = 0; i < n; i++)
			pw.printf( "\tint add%d( int x, int y )\n", i );
		pw.println( "}" );
		pw.close();
	}
}
