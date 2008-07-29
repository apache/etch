/* $Id: CommandParser.java 22291 2006-04-27 20:12:57Z wert $
 *
 * Created by wert on Mar 27, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.cmd;

import java.io.File;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.Vector;

import metreos.util.URL;


/**
 * CommandParser implements standard parsing of command lines.
 * Command line options and parameters are described and then
 * handling of printing usage and help messages is automatic,
 * as is distribution of default option values when none is
 * specified before the first parameter. Data type conversions
 * and value constraint checking is also supported.
 */
public class CommandParser
{
	/**
	 * Constructs a CommandParser for the specified program.
	 * @param program
	 */
	public CommandParser( Program program )
	{
		this.program = program;
	}
	
	/**
	 * Description of prog
	 */
	protected final Program program;
	
	/**
	 * Defines an option. Checks option flags and default value.
	 * @param option
	 */
	public void define( Option option )
	{
		if (option.isHidden() && option.getDefaultValues() == null)
			throw new IllegalArgumentException( "hidden option '"+option+"' has no default value" );
		
		if (option.isHidden() && option.isSingleton())
			throw new IllegalArgumentException( "hidden option '"+option+"' redundantly marked singleton" );
		
		if (option.isRequired() && option.getDefaultValues() != null)
			throw new IllegalArgumentException( "option '"+option+"' with default value redundantly marked required" );
		
		if (!option.isHidden())
		{
			Iterator<String> i = option.getTokens();
			while (i.hasNext())
			{
				String token = i.next();
				Option other = options.put( token, option );
				if (other != null)
					throw new IllegalArgumentException(
						"options '"+option+"' and '"+other+"' have overlapping token sets" );
			}
		}
		
		optionList.add( option );
	}
	
	/**
	 * @param parameter
	 * @param isCatchAll 
	 */
	public void define( Parameter parameter, boolean isCatchAll )
	{
		if (parameter.isRequired() && !lastIsRequired)
			throw new IllegalArgumentException(
				"required parameter '"+parameter+"' cannot follow optional one" );
		
		lastIsRequired  = parameter.isRequired();
		
		if (isCatchAll)
		{
			defineCatchAll( parameter );
			return;
		}
		
		if (catchAllParameter != null)
			throw new UnsupportedOperationException(
				"cannot define parameter '"+parameter+"': catch all parameter already defined" );
		
		parameters.add( parameter );
	}
	
	/**
	 * @param parameter
	 */
	private void defineCatchAll( Parameter parameter )
	{
		if (parameter != null && catchAllParameter != null)
			throw new UnsupportedOperationException(
				"cannot define catch all parameter '"+parameter+"': catch all parameter already defined" );
		
		catchAllParameter = parameter;
	}
	
	/**
	 * @param args
	 * @return true if parsing was sucessful, false otherwise.
	 * @throws Exception
	 */
	public boolean parse( String[] args ) throws Exception
	{
		Iterator<Parameter> parametersIterator = getParameters();
		
		argsIterator = new StringArrayIterator( args );
		while (argsIterator.hasNext())
		{
			String token = argsIterator.next();
			
			Option option = findOption( token );
			if (option != null)
			{
				Object[] values = null;
				
				if (option.wantsValue())
				{
					values = option.getAndConvertValue( token, argsIterator );
					if (values == null)
						return false;
					
					if (!option.checkValue( token, values ))
						return false;
				}
				
				if (!option.deliverValue( token, values, false ))
					return false;
				
				continue;
			}
			
			// ok, we're delivering a non-option value.
			if (!finalizeOptions( " before the first parameter" ))
				return false;
			
			if (parametersIterator.hasNext() || catchAllParameter != null)
			{
				Parameter parameter = parametersIterator.hasNext() ?
					(Parameter) parametersIterator.next() : catchAllParameter;
				
				Object value = parameter.convertValue( token );
				if (value == null)
					return false;
				
				if (!parameter.checkValue( value ))
					return false;
				
				if (!parameter.deliverValue( value ))
					return false;
				
				if (parameter == catchAllParameter)
					hasDeliveredToCatchAll = true;
				
				continue;
			}
			
			report( "unexpected command line token", token, argsIterator );
			return false;
		}
		
		// ok, we're about to call run, finalize options
		
		if (!finalizeOptions( " on the command line" ))
			return false;
		
		// check for unspecified but required parameters.
		
		if (parametersIterator.hasNext())
		{
			Parameter parameter = parametersIterator.next();
			if (parameter.isRequired())
			{
				report( "required parameter '"+parameter+"' has not been specified", null, null );
				return false;
			}
		}
		
		if (catchAllParameter != null && catchAllParameter.isRequired() && !hasDeliveredToCatchAll)
		{
			report( "required parameter '"+catchAllParameter+"' has not been specified", null, null );
			return false;
		}
		
		return true;
	}
	
	/**
	 * @return true if there is another token.
	 */
	public boolean hasNextToken()
	{
		return argsIterator.hasNext();
	}
	
	/**
	 * @return the next token.
	 * @throws java.util.NoSuchElementException if there isn't one.
	 */
	public String nextToken()
	{
		return argsIterator.next();
	}
	
	private Iterator<String> argsIterator;
	
	private boolean hasDeliveredToCatchAll = false;

//	/**
//	 * Delivers an option value from another source.
//	 * @param token
//	 * @param strValue
//	 * @return true if the value was delivered to the option, false otherwise.
//	 * @throws Exception 
//	 */
//	public boolean deliverValue( String token, String strValue )
//		throws Exception
//	{
//		Option option = findOption( token );
//		if (option == null)
//			throw new Exception( "no such option '"+token+"'" );
//		
//		if (!option.wantsValue())
//			throw new Exception( "option '"+token+"' does not accept a value" );
//		
//		Object value = option.convertValue( strValue );
//		if (value == null)
//			return false;
//		
//		if (!option.checkValue( token, value ))
//			return false;
//		
//		return option.deliverValue( token, value, false );
//	}
	
	/**
	 * Finalizes the options before some important event (described by when).
	 * 
	 * @param when
	 * 
	 * @return true if the options are ok, false if there is a problem, such
	 * as a required option without a value, etc.
	 * 
	 * @throws Exception
	 */
	public boolean finalizeOptions( String when ) throws Exception
	{
		if (hasFinalizedOptions)
			return true;
		
		hasFinalizedOptions = true;
		
		Iterator<Option> i = getOptionList();
		while (i.hasNext())
		{
			Option option = i.next();
//			System.out.println( "checking option "+option );
			if (!isAlreadySpecified( option, false ))
			{
				if (option.getDefaultValues() != null)
				{
					if (!option.deliverValue( option.getPrimaryToken(),
							option.getDefaultValues(), true ))
						return false;
				}
				else if (option.isRequired())
				{
					// ok, it isn't already specified, it doesn't have a default
					// value, and it is required. we blow beets.
					report( "option '"+option+"' must be specified"+when, null, null );
					return false;
				}
			}
		}
		
		return true;
	}
	
	private boolean hasFinalizedOptions = false;

	/**
	 * @param token
	 * @return the option matching token or null.
	 */
	private Option findOption( String token )
	{
		return getOption( token );
	}
	
	/**
	 * @param msg
	 * @param token
	 * @param i
	 */
	public void report( String msg, String token, Iterator<String> i )
	{
		System.err.print( program );
		System.err.print( ": " );
		System.err.print( msg );
		
		if (token != null)
		{
			System.err.print( ": " );
			System.err.print( token );
			while (i != null && i.hasNext())
			{
				System.err.print( ' ' );
				System.err.print( i.next() );
			}
		}
		
		System.err.println();
		System.err.flush();
	}
	
	private final Map<String,Option> options = new HashMap<String,Option>();
	
	private final List<Option> optionList = new Vector<Option>();
	
	private final List<Parameter> parameters = new Vector<Parameter>();
	
	private Parameter catchAllParameter;

	/**
	 * @param justUsage true if the 
	 */
	public void showHelp( boolean justUsage )
	{
		System.err.print( "usage: " );
		
		System.err.print( program );
		
		{
			Iterator<Option> i = getOptionList();
			while (i.hasNext())
			{
				Option option = i.next();
				if (option.isHidden())
					continue;
				option.showShortDescription();
			}
		}
		
		int brackets = 0;
		
		{
			Iterator<Parameter> i = getParameters();
			while (i.hasNext())
			{
				Parameter parameter = i.next();
				if (!parameter.isRequired())
				{
					brackets++;
					System.err.print( " [" );
				}
				parameter.showShortDescription();
			}
		}
		
		if (catchAllParameter != null)
		{
			if (!catchAllParameter.isRequired())
			{
				brackets++;
				System.err.print( " [" );
			}
			catchAllParameter.showShortDescription();
			System.err.print( " ..." );
		}
		
		while (brackets-- > 0)
			System.err.print( " ]" );
		
		System.err.println();
		
		if (!justUsage)
		{
			{
				Iterator<Option> i = getOptionList();
				while (i.hasNext())
				{
					Option option = i.next();
					if (option.isHidden())
						continue;
					System.err.println();
					option.showLongDescription();
				}
			}
			
			{
				Iterator<Parameter> i = getParameters();
				while (i.hasNext())
				{
					Parameter parameter = i.next();
					System.err.println();
					parameter.showLongDescription();
				}
			}
			
			if (catchAllParameter != null)
			{
				System.err.println();
				catchAllParameter.showLongDescription();
			}
		}
	}

	/**
	 * Checks whether the option has already been specified or
	 * not, and marks it as having been specified.
	 * @param option
	 * @param update
	 * @return true if the option has already been specified (other than by default).
	 */
	public boolean isAlreadySpecified( Option option, boolean update )
	{
		if (specifiedOptions.contains( option ))
			return true;
		
		if (update)
			specifiedOptions.add( option );
		
		return false;
	}
	
	private Set<Option> specifiedOptions = new HashSet<Option>();

	private boolean lastIsRequired = true;

	/**
	 * @return an iterator over parameters.
	 */
	private Iterator<Parameter> getParameters()
	{
		return parameters.iterator();
	}

	/**
	 * @param token
	 * @return the option for the specified token, or null.
	 */
	public Option getOption( String token )
	{
		return options.get( token );
	}
	
	private Iterator<Option> getOptionList()
	{
		return optionList.iterator();
	}

	////////////////////////////////
	// DEFINE PARAMETER SHORTCUTS //
	////////////////////////////////
	
	/**
	 * @param name
	 * @param method
	 * @param description
	 * @param isRequired 
	 * @param isCatchAll
	 * @param constraint
	 * @throws Exception
	 */
	public void defineBooleanParameter( String name, String method, String description,
		boolean isRequired, boolean isCatchAll, Constraint constraint ) throws Exception
	{
		define( new BooleanParameter( this, name, method, description,
			isRequired, constraint ), isCatchAll );
	}
	
	/**
	 * @param name
	 * @param method
	 * @param description
	 * @param isRequired 
	 * @param isCatchAll
	 * @param constraint
	 * @throws Exception
	 */
	public void defineDoubleParameter( String name, String method, String description,
		boolean isRequired, boolean isCatchAll, Constraint constraint ) throws Exception
	{
		define( new DoubleParameter( this, name, method, description,
			isRequired, constraint ), isCatchAll );
	}
	
	/**
	 * @param name
	 * @param method
	 * @param description
	 * @param isRequired 
	 * @param isCatchAll
	 * @param constraint
	 * @throws Exception
	 */
	public void defineFileParameter( String name, String method, String description,
		boolean isRequired, boolean isCatchAll, Constraint constraint ) throws Exception
	{
		define( new FileParameter( this, name, method, description,
			isRequired, constraint ), isCatchAll );
	}
	
	/**
	 * @param name
	 * @param method
	 * @param description
	 * @param isRequired 
	 * @param isCatchAll
	 * @param constraint
	 * @throws Exception
	 */
	public void defineIntegerParameter( String name, String method, String description,
		boolean isRequired, boolean isCatchAll, Constraint constraint ) throws Exception
	{
		define( new IntegerParameter( this, name, method, description,
			isRequired, constraint ), isCatchAll );
	}
	
	/**
	 * @param name
	 * @param method
	 * @param description
	 * @param isRequired 
	 * @param isCatchAll
	 * @param constraint
	 * @throws Exception
	 */
	public void defineStringParameter( String name, String method, String description,
		boolean isRequired, boolean isCatchAll, Constraint constraint ) throws Exception
	{
		define( new StringParameter( this, name, method, description,
			isRequired, constraint ), isCatchAll );
	}
	
	/**
	 * @param name
	 * @param method
	 * @param description
	 * @param isRequired 
	 * @param isCatchAll
	 * @param constraint
	 * @throws Exception
	 */
	public void defineUrlParameter( String name, String method, String description,
		boolean isRequired, boolean isCatchAll, Constraint constraint ) throws Exception
	{
		define( new UrlParameter( this, name, method, description,
			isRequired, constraint ), isCatchAll );
	}

	/////////////////////////////
	// DEFINE OPTION SHORTCUTS //
	/////////////////////////////

	/**
	 * @param tokens
	 * @param name
	 * @param method
	 * @param description
	 * @param flags
	 * @param defaultValue
	 * @param constraint
	 * @throws Exception
	 */
	public void defineBooleanOption( String tokens, String name, String method,
		String description, int flags, Boolean defaultValue, Constraint constraint )
		throws Exception
	{
		define( new BooleanOption( this, tokens, name, method, description, flags,
			defaultValue, constraint ) );
	}

	/**
	 * @param tokens
	 * @param name
	 * @param method
	 * @param description
	 * @param flags
	 * @param defaultValue
	 * @param constraint
	 * @throws Exception
	 */
	public void defineDoubleOption( String tokens, String name, String method,
		String description, int flags, Double defaultValue, Constraint constraint )
		throws Exception
	{
		define( new DoubleOption( this, tokens, name, method, description, flags,
			defaultValue, constraint ) );
	}

	/**
	 * @param tokens
	 * @param name
	 * @param method
	 * @param description
	 * @param flags
	 * @param defaultValue
	 * @param constraint
	 * @throws Exception
	 */
	public void defineFileOption( String tokens, String name, String method,
		String description, int flags, File defaultValue, Constraint constraint )
		throws Exception
	{
		define( new FileOption( this, tokens, name, method, description, flags,
			defaultValue, constraint ) );
	}

	/**
	 * Method should be declared as:
	 * 
	 * public boolean method( CommandParser cp, Option option, String token, Integer value )
	 * 
	 * @param tokens
	 * @param name
	 * @param method
	 * @param description
	 * @param flags
	 * @param defaultValue
	 * @param constraint
	 * @throws Exception
	 */
	public void defineIntegerOption( String tokens, String name, String method,
		String description, int flags, Integer defaultValue, Constraint constraint )
		throws Exception
	{
		define( new IntegerOption( this, tokens, name, method, description, flags,
			defaultValue, constraint ) );
	}

	/**
	 * @param tokens
	 * @param method
	 * @param description
	 * @param flags
	 * @throws Exception
	 */
	public void defineNullOption( String tokens, String method, String description,
		int flags ) throws Exception
	{
		define( new NullOption( this, tokens, method, description, flags ) );
	}

	/**
	 * @param tokens
	 * @param name
	 * @param method
	 * @param description
	 * @param flags
	 * @param defaultValue
	 * @param constraint
	 * @throws Exception
	 */
	public void defineStringOption( String tokens, String name, String method,
		String description, int flags, String defaultValue, Constraint constraint )
		throws Exception
	{
		define( new StringOption( this, tokens, name, method, description,
			flags, defaultValue, constraint ) );
	}

	/**
	 * @param tokens
	 * @param names
	 * @param method
	 * @param description
	 * @param flags
	 * @param defaultValues 
	 * @param constraints 
	 * @throws Exception 
	 */
	public void defineStringArrayOption( String tokens, String[] names,
		String method, String description, int flags, String[] defaultValues,
		Constraint[] constraints ) throws Exception
	{
		define( new StringArrayOption( this, tokens, names, method,
			description, flags, defaultValues, constraints ) );
	}

	/**
	 * @param tokens
	 * @param name
	 * @param method
	 * @param description
	 * @param flags
	 * @param defaultValue
	 * @param constraint
	 * @throws Exception
	 */
	public void defineUrlOption( String tokens, String name, String method,
		String description, int flags, URL defaultValue, Constraint constraint )
		throws Exception
	{
		define( new UrlOption( this, tokens, name, method, description, flags,
			defaultValue, constraint ) );
	}
}
