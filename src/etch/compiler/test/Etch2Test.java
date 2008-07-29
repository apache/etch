package etch.compiler.test;

import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;

import etch.compiler.Backend;
import etch.compiler.CmdLineOptions;
import etch.compiler.CompilerLogHandler;
import etch.compiler.Etch2;
import etch.compiler.LogHandler;
/**
 * Driver to test Compiler API
 * @author gsandhir
 *
 */
public class Etch2Test
{
	private CmdLineOptions cmdLineOption;
	private LogHandler logHandler;
	
	/**
	 * Constructor sets up the Command Line Options
	 *
	 */
	public Etch2Test()
	{
		cmdLineOption = new CmdLineOptions();
		cmdLineOption.binding = Backend.BINDING_JAVA;
		HashSet<String> set = new HashSet<String>();
		set.add(Backend.WHAT_ALL  );
		cmdLineOption.what = set;
		File outputDir = new File( "C:\\demo" );
		File includeFileLocation = new File( "C:\\demo\\include");
		cmdLineOption.outputDir = outputDir;
		List<File> etchIncludePath = new LinkedList<File>();
		etchIncludePath.add( includeFileLocation);
		CmdLineOptions.includePath = etchIncludePath;
		logHandler = new CompilerLogHandler();
	}
	
	/**
	 * Compiles Etch String and generates code
	 * @param str  Etch String
	 */

	public void generateFromString(String str) {
		logHandler.push( str, null );
		InputStream is1 = new ByteArrayInputStream(str.getBytes());
		Etch2.doCompile( cmdLineOption, is1, logHandler );	
		logHandler.pop();
	}
	/**
	 * Compiles Etch String and generates code
	 * @param fileName Etch file
	 */
	public void generateFromFile(String fileName) {
		File etchFile = new File(fileName);
		logHandler.push( etchFile.getName(), null );
		InputStream is;
		try
		{
			is = new java.io.FileInputStream( etchFile );
		}
		catch ( FileNotFoundException e1 )
		{			
			e1.printStackTrace();
			System.exit( 1 );
			return;
		}
		Etch2.doCompile( cmdLineOption, is, logHandler );	
		logHandler.pop();
	} 
	/**
	 * Main Program to test Etch API
	 * @param args
	 */
		
	public static void main( String[] args) {
		Etch2Test etchTest = new Etch2Test();
		if (args.length != 2) {
			System.out.println( " Usage: Etch2Test -f [EtchFilename ] | -s [EtchString]");
			return;
		}
		
			String firstTokenString = args[0];
			if (firstTokenString.equalsIgnoreCase( "-f" )) {
				etchTest.generateFromFile(args[1]);
			}
			else if (firstTokenString.equalsIgnoreCase( "-s" )) {
				etchTest.generateFromString( args[1] );
			}
			else {
				System.out.println( " Usage: Etch2Test -f [EtchFilename ] | -s [EtchString]");
			}
				
		
		
//		etchTest.generateFromFile("C:\\demo\\Example.etch");
//		etchTest.generateFromString("module foo service bar {  void hello( int x, int y ) }");		
	}
	
	
	
}
