/* $Id: Program.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Mar 27, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.cmd;

/**
 * Description of Program
 */
abstract public class Program
{
	/**
	 * @param program
	 * @param args
	 * @throws Exception
	 */
	protected static void main( final Program program, String[] args ) 
		throws Exception
	{
		CommandParser cp = new CommandParser( program );
		
		cp.defineNullOption( program.getHelpTokens(), "doHelp",
			"print a description of the command options and parameters and then exit.",
			Option.NONE );
		
		program.defineOptionsAndParameters( cp );
		
		if (!cp.parse( args ))
			System.exit( 1 );
		
		Runtime.getRuntime().addShutdownHook(
			new Thread()
			{
				@Override
				public void run()
				{
					program.shutdown( program.shutdownSource,
						program.shutdownReason, program.shutdownEmergency );
				}
			});
		
		program.run();
	}

	/**
	 * @param code the exit code.
	 * @param source the source of the request.
	 * @param reason the reason for the request.
	 * @param emergency perform minimal shutdown if true.
	 */
	public void exit( int code, String source, String reason,
		boolean emergency )
	{
		shutdownSource = source;
		shutdownReason = reason;
		shutdownEmergency = emergency;
		System.exit( code );
	}
	
	/**
	 * The source of the request.
	 */
	String shutdownSource = "runtime";
	
	/**
	 * The reason for the request.
	 */
	String shutdownReason = "unknown";
	
	/**
	 * Full shutdown (false) or minimal (true)?
	 */
	boolean shutdownEmergency = false;

	/**
	 * @return the list of tokens the user might specify to get help.
	 * The default list of tokens is "--help|-help|-h|?".
	 */
	protected String getHelpTokens()
	{
		return "--help|-help|-h|?";
	}

	/**
	 * Prints the help message for the program.
	 * @param cp
	 * @param option
	 * @param token
	 * @return false indicating the program should exit.
	 */
	public boolean doHelp( CommandParser cp, Option option, String token )
	{
		cp.showHelp( false );
		return false;
	}

	@Override
	public String toString()
	{
		return getClass().getName().toString();
	}
	
	/**
	 * Gives the program a chance to define its options and parameters.
	 * @param cp
	 * @throws Exception
	 */
	abstract protected void defineOptionsAndParameters( CommandParser cp )
		throws Exception;
	
	/**
	 * Runs the program after successful command line parsing.
	 * @throws Exception
	 */
	abstract protected void run() throws Exception;
	
	/**
	 * Shuts down the program in an orderly way when System.exit is called,
	 * or when the program is killed by external command (^C) or unix kill.
	 * @param source the source of the request
	 * @param reason the reason for the shutdown 
	 * @param emergency only a minimal amount of shutdown should be performed
	 */
	protected void shutdown( String source, String reason, boolean emergency )
	{
		// nothing to do.
	}
}
