/* $Id$
 *
 * Created by wert on Apr 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStream;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;
import java.util.StringTokenizer;

import etch.compiler.ast.Module;
import etch.util.cmd.CommandParser;
import etch.util.cmd.Option;
import etch.util.cmd.Parameter;
import etch.util.cmd.Program;


/**
 * The main command line interface for the etch compiler.
 */
public class Etch2 extends Program
{
	private static String WHO_DELIMETER = ",";

	private static String ETCH_INCLUDE_PATH = "ETCH_INCLUDE_PATH";

	private LogHandler logHandler;

	/**
	 * @param args
	 */
	public static void main( String[] args )
	{
		try
		{
			main( new Etch2(), args );
		}
		catch ( Throwable e )
		{
			// TODO can we be more descriptive here?
			e.printStackTrace();
			System.exit( 3 );
			return;
		}
	}

	/**
	 * Constructs the Etch2 parser / compiler driver.
	 */
	public Etch2()
	{
		// Default options
		ignoreGlobal = false;
		ignoreLocal = false;
		ignoreEnvIncludePath = false;

		includePath = new LinkedList<File>();
		logHandler = new CompilerLogHandler( "Command" );
		
		logHandler.setQuiet( false );
	}

//	/**
//	 * Print an informative message to standard out unless running in quiet
//	 * mode.
//	 * @param fmt the format of the message
//	 * @param args the contents of the message
//	 */
//	private void report( String fmt, Object ... args )
//	{
//		if (!quiet)
//			System.out.printf( "Etch: %s\n", String.format( fmt, args ) );
//	}


//	/**
//	 * Print an informative message to standard error unless running in quiet
//	 * mode.
//	 * @param fmt the format of the message
//	 * @param args the contents of the message
//	 */
//	private void error( String fmt, Object ... args )
//	{
//		System.err.printf( "Etch: %s\n", String.format( fmt, args ) );
//	}
	
	@Override
	public String toString()
	{
		return "etch";
	}

	@Override
	protected void defineOptionsAndParameters( CommandParser cp )
		throws Exception
	{
		cp.defineFileOption( "-d|--output-dir", "outputDir", "setOutputDir",
			"specifies the output directory",
			Option.SINGLETON, null, null );

		cp.defineStringOption( "-b|--binding", "binding", "setBinding",
			"specifies a target language binding to generate",
			Option.REQUIRED|Option.SINGLETON, null, null );

		cp.defineStringOption( "-w|--what|--who", "what", "setWhat",
			"specifies what files to generate (--who is deprecated)",
			Option.SINGLETON, "BOTH", null );

		cp.defineNullOption( "-g|--ignore-global", "ignoreGlobalWordsList",
			"ignore the global reserved words list",
			Option.SINGLETON );

		cp.defineNullOption( "-l|--ignore-local", "ignoreLocalWordsList",
			"ignore the local reserved words list",
			Option.SINGLETON );

		cp.defineFileOption( "-W|--word-list", "wordList", "setUserWordsList",
			"the file name of a reserved words list",
			Option.SINGLETON, null, null );

		cp.defineStringOption( "-I|--include-path", "includePath", "setIncludePath",
			"a search directory for included etch files",
			Option.NONE, null, null );

		cp.defineNullOption( "-i|--ignore-include-path", "ignoreEnvIncludePath",
			"ignore the include path environment variable",
			Option.SINGLETON );
		
		cp.defineFileOption( "-m|--dir-mixin", "mixinOutputDir", "setMixinOutputDir",
			"the output dir for mixin artifacts",
			Option.SINGLETON, null, null );
		
		cp.defineNullOption( "-n|--no-mixin-artifacts", "setNoMixinArtifacts",
			"mixin artifacts should not be generated",
			Option.SINGLETON );

		cp.defineNullOption( "-q|--quiet", "setQuiet",
			"only report problems",
			Option.SINGLETON );

		cp.defineNullOption( "-v|--version", "doVersion",
			"show the version and exit",
			Option.SINGLETON );
		
		cp.defineNullOption( "-f|--no-flatten", "setNoFlattenPackages",
			"the namespace directory tree should not be flattened",
			Option.SINGLETON );

		cp.defineFileParameter( "file", "doCompile",
			"Etch source to compile", true, true, null );
	}

	/**
	 * Command parser method to set the output directory.
	 * @param cp
	 * @param option
	 * @param token
	 * @param value the output directory
	 * @return true if it worked, false otherwise.
	 */
	public boolean setOutputDir( CommandParser cp, Option option, String token,
		File value )
	{
		if (!value.isDirectory() || !value.canWrite())
		{
			String msg = String.format("output directory %s is not found or not writable.", value);
		//	error( "output directory %s is not found or not writable.", value );
			logHandler.logMessage( LogHandler.LEVEL_ERROR, null, msg );
			return false;
		}
		outputDir = value;
		return true;
	}

	private File outputDir;

	/**
	 * Command parser method to set the binding.
	 * @param cp
	 * @param option
	 * @param token
	 * @param value the binding
	 * @return true if it worked, false otherwise.
	 */
	public boolean setBinding( CommandParser cp, Option option, String token,
		String value )
	{
		try
		{
			binding = Class.forName( String.format(
				"etch.bindings.%s.compiler.Compiler", value ) );
			return true;
		}
		catch ( ClassNotFoundException e )
		{
			String msg = String.format("binding %s is not found.", value);
			logHandler.logMessage( LogHandler.LEVEL_ERROR, null, msg );
			return false;
		}
	}

	private Class<?> binding;

	/**
	 * Command parser method to set for whom/what we are generating code.
	 * @param cp
	 * @param option
	 * @param token
	 * @param value the list of what needs to be generated.
	 * @return true if it worked, false otherwise.
	 */
	public boolean setWhat( CommandParser cp, Option option, String token,
		String value )
	{
		try
		{
			StringTokenizer st = new StringTokenizer(value,WHO_DELIMETER);
			while (st.hasMoreElements()) {
				what.add(st.nextToken().toUpperCase());
			}
			return true;
		}
		catch ( Exception e )
		{
			String msg = String.format("what %s is not known.", value);
			logHandler.logMessage( LogHandler.LEVEL_ERROR, null, msg );
			return false;
		}
	}

	private Set<String> what = new HashSet<String>();

	/**
	 * Command parser method to ignore the globally reserved words list.
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void ignoreGlobalWordsList(CommandParser cp, Option option, String token)
	{
		ignoreGlobal = true;
	}

	private boolean ignoreGlobal;

	/**
	 * Command parser method to ignore the locally reserved words list.
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void ignoreLocalWordsList(CommandParser cp, Option option, String token)
	{
		ignoreLocal = true;
	}

	private  boolean ignoreLocal;

	/**
	 * Command parser method to set the user-defined reserved words list.
	 * @param cp
	 * @param option
	 * @param token
	 * @param value the reserved words list
	 * @return true if it worked, false otherwise.
	 */
	public boolean setUserWordsList(CommandParser cp, Option option, String token,
		File value)
	{
		if (!value.isFile() || !value.canRead())
		{
			String msg = String.format("word list %s is not found or not readable.", value);
			logHandler.logMessage( LogHandler.LEVEL_ERROR, null, msg );
			return false;
		}
		userWordsList = value.getPath();
		return true;
	}

	private String userWordsList;

	/**
	 * Command parser method to set the include path.
	 * @param cp
	 * @param option
	 * @param token
	 * @param value path to append to the current include path
	 */
	public void setIncludePath( CommandParser cp, Option option, String token,
		String value )
	{
		StringTokenizer includeTokens = new StringTokenizer(value,
			File.pathSeparator);

		while(includeTokens.hasMoreTokens())
		{
			File temp = new File(includeTokens.nextToken());
			if (temp.exists() && temp.isDirectory())
				includePath.add(temp);
		}
	}

	private List<File> includePath;

	/**
	 * Command parser method to ignore the include path from the environment.
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void ignoreEnvIncludePath(CommandParser cp, Option option,
		String token)
	{
		ignoreEnvIncludePath = true;
	}

	private boolean ignoreEnvIncludePath;

	/**
	 * Command parser method to set the quiet flag.
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void setQuiet( CommandParser cp, Option option, String token )
	{
//		quiet = true;
		logHandler.setQuiet( true );
	}

	// TODO quiet needs to be included in the options and log handler should respect it.
//	@SuppressWarnings("unused")
//	private boolean quiet;

	/**
	 * Command parser method to show the version and exit.
	 * @param cp
	 * @param option
	 * @param token
	 * @return false so that we stop.
	 */
	public boolean doVersion( CommandParser cp, Option option, String token )
	{
		logHandler.logMessage( LogHandler.LEVEL_MESSAGE, null,
			String.format( "Etch version is %s.", Version.VERSION ) );
		return false;
	}
	
	/**
	 * Command parser method to suppress mixin artifacts.
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void setNoMixinArtifacts( CommandParser cp, Option option, String token )
	{
		isMixinSuppressed = true;
	}

	private boolean isMixinSuppressed = false;
	
	/**
	 * Command parser method to set the output directory for mixins.
	 * @param cp
	 * @param option
	 * @param token
	 * @param value the output directory for mixin
	 * @return true if it worked, false otherwise.
	 */
	public boolean setMixinOutputDir( CommandParser cp, Option option, String token,
		File value )
	{
		if (!value.isDirectory() || !value.canWrite())
		{
			String msg = String.format(" Mixin Output Directory %s is not found or not writable.", value);
		//	error( "output directory %s is not found or not writable.", value );
			logHandler.logMessage( LogHandler.LEVEL_ERROR, null, msg );
			return false;
		}
		mixinOutputDir = value;
		return true;
	}

	private File mixinOutputDir;
	
	/**
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void setNoFlattenPackages( CommandParser cp, Option option, String token)
	{
		flattenPackages = false;
	}
	
	private boolean flattenPackages = true;

	/**
	 * Command parser method to compile a file.
	 * @param cp
	 * @param param
	 * @param value
	 * @return true if it worked, false otherwise.
	 */
	public boolean doCompile( CommandParser cp, Parameter param, File value )
	{
		logHandler.push( value.getName(),null );
		if (!value.isFile() || !value.canRead())
		{
			String msg = String.format("file %s is not found or not readable.", value);
			logHandler.logMessage( LogHandler.LEVEL_ERROR, null, msg );
			return false;
		}

		String s = value.getName();
		if (!s.endsWith( ".etch" ))
		{
			String msg = String.format("file %s is not etch source (its name doesn't end in .etch).", value);
			logHandler.logMessage( LogHandler.LEVEL_ERROR,null, msg );
			return false;
		}

		InputStream is;
		try
		{
			is = new java.io.FileInputStream( value );
		}
		catch ( FileNotFoundException e1 )
		{
			e1.printStackTrace();
			String msg = String.format("file %s could not be opened.", value);
			logHandler.logMessage( LogHandler.LEVEL_ERROR, null, msg );
			return false;
		}
		
		logHandler.setPhase( "Parsing" );

		if (outputDir != null) {
			String msg = String.format(" Compiling to %s ...\n", outputDir);
			logHandler.logMessage( LogHandler.LEVEL_INFO, null, msg );
		}
		else
			logHandler.logMessage( LogHandler.LEVEL_INFO, null, " Compiling.." );

		// Setup Etch include path from the environment variable, command
		// line options, and the working directory of the etch file itself.
		String tempIncPathStr = System.getenv(ETCH_INCLUDE_PATH);
		if (!ignoreEnvIncludePath && tempIncPathStr != null)
		{
			List<File> etchIncludePath = new LinkedList<File>();
			StringTokenizer tempPath = new StringTokenizer(tempIncPathStr,
				File.pathSeparator);

			while (tempPath.hasMoreTokens())
			{
				File temp = new File(tempPath.nextToken());
				if (temp.exists() && temp.isDirectory())
					etchIncludePath.add(temp);
			}

			etchIncludePath.addAll( includePath );
			includePath = etchIncludePath;
		}

		// Directory of etch file comes first.
		if (value.getParentFile() != null) {
			includePath.add(0, value.getParentFile());
		}
		else {
			includePath.add( 0, value.getAbsoluteFile().getParentFile());
		}

		// Construct the Command Line Options
		CmdLineOptions cmdOptions = new CmdLineOptions();
		cmdOptions.bindingClass = binding;
		cmdOptions.what = what;
		cmdOptions.outputDir = outputDir;
		cmdOptions.ignoreGlobal = ignoreGlobal;
		cmdOptions.ignoreLocal = ignoreLocal;
		cmdOptions.userWordsList = userWordsList;
		CmdLineOptions.includePath = includePath;
		cmdOptions.mixinOutputDir = mixinOutputDir;
		cmdOptions.isMixinSuppressed = isMixinSuppressed;
		cmdOptions.flattenPackages = flattenPackages;
		doCompile(cmdOptions,is,logHandler);
		logHandler.pop();
		if (logHandler.hasError())
			return false;
		
		return true;
	}

	/**
	 * Compile etch source based on command line options.
	 * @param cl the user-defined options
	 * @param is
	 * @param lh
	 * @return the generated Module abstract syntax tree.
	 */
	public static Module doCompile(CmdLineOptions cl, InputStream is, LogHandler lh)
	{
		EtchHelper.lh = lh;
		
		EtchHelper.cl = cl;

		if (!initBindingClass( cl, lh ))
			return null;

		final Backend b = getBinding( cl, lh );
		if (b == null)
			return null;
		
		final EtchGrammar parser = new EtchGrammar( b, is );

		final Module m;

		try
		{
			m = parser.module();

			lh.logMessage( LogHandler.LEVEL_INFO,null,"Parsed OK. \n" );

			m.check();

			lh.logMessage( LogHandler.LEVEL_INFO,null," Checked OK. \n" );
		}
		catch ( ParseException e )
		{

			String fmt =  e.getMessage();
			lh.logMessage( LogHandler.LEVEL_ERROR, e.currentToken,fmt );
	//		System.exit( 2 );
			return null;
		}
		catch ( org.apache.velocity.exception.MethodInvocationException e )
		{
			String fmt =  e.getMessage();
			lh.logMessage( LogHandler.LEVEL_ERROR, null, fmt );
			e.printStackTrace();
	//		System.exit( 3 );
			return null;
		}
		catch ( Exception e )
		{
			String fmt =  e.getMessage();
			lh.logMessage( LogHandler.LEVEL_ERROR, null,fmt );
			e.printStackTrace();
	//		System.exit( 3 );
			return null;
		}
		
		// handling of Mixin output Directory
		
		if (cl.isMixinPresent) {
			if (cl.mixinOutputDir != null) {
				cl.outputDir = cl.mixinOutputDir;
			}
		}
	
		try
		{
			// TODO report word lists too?
			lh.logMessage( LogHandler.LEVEL_INFO,null,  " Generating Resources... \n" );

			// TODO integrate includePath with code generation.
			b.generate( m, cl, lh );
		}
		catch ( Exception e )
		{
			String fmt =  e.getMessage();
			lh.logMessage( LogHandler.LEVEL_ERROR, null, fmt );
//			e.printStackTrace( System.err );
//			System.exit( 3 );
			return null;
		}

		lh.logMessage( LogHandler.LEVEL_INFO,null, " Generation Done... \n" );
		return m;
	}

	private static Backend getBinding( CmdLineOptions cl, LogHandler lh )
	{
		try
		{
			return (Backend) cl.bindingClass.newInstance();
		}
		catch ( Exception e )
		{
			String fmt =  e.getMessage();
			lh.logMessage( LogHandler.LEVEL_ERROR, null, fmt );
			e.printStackTrace( System.err );
	//		System.exit( 3 );
			return null;
		}
	}

	private static boolean initBindingClass( CmdLineOptions cl, LogHandler lh )
	{
		if (cl.bindingClass != null)
			return true;

		try
		{
			cl.bindingClass = Class.forName( String.format(
				"etch.bindings.%s.compiler.Compiler", cl.binding ) );
			return true;
		}
		catch ( ClassNotFoundException e )
		{
			String fmt = "Binding  " + cl.binding + " is not supported \n";
			lh.logMessage( LogHandler.LEVEL_ERROR, null, fmt );
			// e.printStackTrace();
			return false;
		}
		catch ( Exception ex )
		{
			String fmt = ex.getMessage();
			lh.logMessage( LogHandler.LEVEL_ERROR, null, fmt );
			ex.printStackTrace( System.err );
			return false;
		}
	}

	@Override
	protected void run() throws Exception
	{
		// nothing to do...
	}
}
