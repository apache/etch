/* $Id$
 *
 * Created by wert on Apr 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler;

import java.io.File;
import java.util.HashSet;
import java.util.Set;
import java.util.StringTokenizer;

import metreos.cmd.CommandParser;
import metreos.cmd.Option;
import metreos.cmd.Parameter;
import metreos.cmd.Program;
import etch.compiler.ast.Module;

/**
 * The main cli interface to the etch compiler.
 */
public class Etch2 extends Program
{
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
		// Set default reserved words lists
		ignoreGlobal = false;
		ignoreLocal = false;
		userWordsList = null;
	}

	private boolean ignoreGlobal;

	private  boolean ignoreLocal;

	private String userWordsList;
	
	private String WHO_DELIMETER = ",";

	private void report( String fmt, Object ... args )
	{
		if (!quiet)
			System.out.printf( "Etch: %s\n", String.format( fmt, args ) );
	}

	private void error( String fmt, Object ... args )
	{
		System.err.printf( "Etch: %s\n", String.format( fmt, args ) );
	}

	@Override
	protected void defineOptionsAndParameters( CommandParser cp )
		throws Exception
	{
		cp.defineFileOption( "-d|--outputDir", "outputDir", "setOutputDir",
			"specifies the output directory", Option.SINGLETON, null, null );

		cp.defineStringOption( "-b|--binding", "binding", "setBinding",
			"selects a target language binding to generate", Option.SINGLETON,
			null, null );
		
		cp.defineStringOption( "-w|--who|--what", "who", "setWhat",
			"selects who to generate for", Option.NONE,
			"BOTH", null );

		cp.defineNullOption( "-ig|--ignoreGlobal", "ignoreGlobalWordsList",
			"ignore the global reserved words list", Option.SINGLETON );

		cp.defineNullOption( "-il|--ignoreLocal", "ignoreLocalWordsList",
			"ignore the local reserved words list", Option.SINGLETON );

		cp.defineFileOption( "-wl|--wordList", "wordList", "setUserWordsList",
			"specifies the file name of a reserved word list to load, " +
			"either augmenting or replacing the global and local word lists",
			Option.SINGLETON, null, null );

		cp.defineNullOption( "-q|--quiet", "setQuiet",
			"only report problems", Option.SINGLETON );

		cp.defineNullOption( "--version", "doVersion",
			"shows the version and exits", Option.NONE );

		cp.defineFileParameter( "file", "doCompile",
			"Etch source to compile", true, true, null );
	}

	/**
	 * Command parser method to set the output dir.
	 * @param cp
	 * @param option
	 * @param token
	 * @param value
	 */
	public void setOutputDir( CommandParser cp, Option option, String token,
		File value )
	{
		if (!value.isDirectory() || !value.canWrite())
		{
			error( "output directory %s is not found or not writable.", value );
			System.exit( 1 );
			return;
		}
		outputDir = value;
	}

	private File outputDir;

	/**
	 * Command parser method to set the binding.
	 * @param cp
	 * @param option
	 * @param token
	 * @param value
	 */
	public void setBinding( CommandParser cp, Option option, String token,
		String value )
	{
		try
		{
			binding = Class.forName( String.format(
				"etch.bindings.%s.compiler.Compiler", value ) );
		}
		catch ( ClassNotFoundException e )
		{
			error( "binding %s is not found.", value );
			System.exit( 1 );
			return;
		}
	}

	private Class<?> binding;

	/**
	 * Command parser method to set who we are generating code for.
	 * @param cp
	 * @param option
	 * @param token
	 * @param value
	 */
	public void setWhat( CommandParser cp, Option option, String token,
		String value )
	{
		try
		{
			StringTokenizer st = new StringTokenizer(value,WHO_DELIMETER);
			while (st.hasMoreElements()) {
				what.add(st.nextToken().toUpperCase());
			}			
		}
		catch ( Exception e )
		{
			error( "what %s is not known.", value );
			System.exit( 1 );
			return;
		}
	}

//	private MessageDirection who;
	private Set<String> what = new HashSet<String>();

	/**
	 *
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void ignoreGlobalWordsList(CommandParser cp, Option option, String token)
	{
		ignoreGlobal = true;
	}

	/**
	 *
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void ignoreLocalWordsList(CommandParser cp, Option option, String token)
	{
		ignoreLocal = true;
	}

	/**
	 *
	 * @param cp
	 * @param option
	 * @param token
	 * @param value
	 */
	public void setUserWordsList(CommandParser cp, Option option, String token,
		File value)
	{
		if (!value.isFile() || !value.canRead())
		{
			error( "word list %s is not found or not readable.", value );
			System.exit( 1 );
			return;
		}
		userWordsList = value.getPath();
	}

	/**
	 * Command parser method to set the quiet flag.
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void setQuiet( CommandParser cp, Option option, String token )
	{
		quiet = true;
	}

	private boolean quiet;

	/**
	 * Command parser method to show the version and exit.
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void doVersion( CommandParser cp, Option option, String token )
	{
		error( "version is %s.", Version.VERSION );
		System.exit( 1 );
		return;
	}

	/**
	 * Command parser method to compile a file.
	 * @param cp
	 * @param param
	 * @param value
	 */
	public void doCompile( CommandParser cp, Parameter param, File value )
	{
		if (!value.isFile() || !value.canRead())
		{
			error( "file %s is not found or not readable.", value );
			System.exit( 1 );
			return;
		}

		String s = value.getName();
		if (!s.endsWith( ".etch" ))
		{
			error( "file %s is not etch source (its name doesn't end in .etch).", value );
			System.exit( 1 );
			return;
		}

		final EtchGrammar parser;

		try
		{
			parser = new EtchGrammar( new java.io.FileInputStream( value ) );
		}
		catch ( java.io.FileNotFoundException e )
		{
			error( "file %s could not be opened.", value );
			System.exit( 1 );
			return;
		}

		if (outputDir != null)
			report( "%s: compiling to %s ...", value, outputDir );
		else
			report( "%s: compiling ...", value );

		final Module m;

		try
		{
			m = parser.module();

			report( "%s: parsed ok.", value );

			m.check();

			report( "%s: checked ok.", value );
		}
		catch ( ParseException e )
		{
			error( "%s: encountered error during parse.\n%s", value, e );
			System.exit( 2 );
			return;
		}
		catch ( org.apache.velocity.exception.MethodInvocationException e )
		{
			Throwable c = e.getCause();

			if (c instanceof ParseException)
			{
				error( "%s: encountered error during parse.\n%s", value, e );
				System.exit( 2 );
				return;
			}

			error( "%s: encountered error during parse.", value );
			c.printStackTrace( System.err );
			System.exit( 3 );
			return;
		}
		catch ( Exception e )
		{
			error( "%s: encountered error during parse.", value );
			e.printStackTrace( System.err );
			System.exit( 3 );
			return;
		}

		if (binding == null)
		{
			report( "%s: no binding is selected, hence no output.", value );
			return;
		}

		final Backend b;

		try
		{
			b = (Backend) binding.newInstance();
		}
		catch ( Exception e )
		{
			error( "%s: encountered error instantiating backend %s.", value, binding );
			e.printStackTrace( System.err );
			System.exit( 3 );
			return;
		}

		try
		{
			// TODO report word lists too?
			report( "generating resources (%s)...", what );
			b.generate( m, what, outputDir, ignoreGlobal, ignoreLocal, userWordsList );
		}
		catch ( Exception e )
		{
			error( "%s: encountered error during %s generation.", value, b );
			e.printStackTrace( System.err );
			System.exit( 3 );
			return;
		}

		report( "%s: %s generation done.", value, b );
	}

	@Override
	protected void run() throws Exception
	{
		// nothing to do...
	}
}
