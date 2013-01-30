/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.util.cmd;

import java.io.File;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.regex.Pattern;

import org.apache.etch.util.CharIterator;
import org.apache.etch.util.StringUtil;
import org.apache.etch.util.URL;


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
		// hidden options are just that, hidden, they aren't required. hidden means
		// help doesn't show them. they can still be specified.
//		if (option.isHidden() && option.getDefaultValues() == null)
//			throw new IllegalArgumentException( "hidden option '"+option+"' has no default value" );

		// adding hidden to an option just means it isn't shown in the help. it
		// you shouldn't have to otherwise edit it. rude it is, however, to hide
		// a required option with no default value.
//		if (option.isHidden() && option.isSingleton())
//			throw new IllegalArgumentException( "hidden option '"+option+"' redundantly marked singleton" );
		
		if (option.isRequired() && option.getDefaultValues() != null)
			throw new IllegalArgumentException( "option '"+option+"' with default value redundantly marked required" );
		
		// we still want to process hidden options, just not show them.
		if (true /* !option.isHidden() */)
		{
			Iterator<String> i = option.getTokens();
			while (i.hasNext())
			{
				String token = i.next();
				
				if (token.startsWith( "--" ))
				{
					// long option (e.g., --name)

					String name = token.substring( 2 );

					// rule out empty name
					if (name.length() == 0)
						throw new IllegalArgumentException( "long option token '"+token+"' has no chars after the dashes" );
					
					// rule out three or more dashes
					if (name.startsWith( "-" ))
						throw new IllegalArgumentException( "long option token '"+token+"' starts with three or more dashes" );
					
					checkValidChars( name, "long option", token, longTokenChars );
					defineOption( options, name, option );
					continue;
				}
				
				if (!token.startsWith( "-" ))
					throw new IllegalArgumentException( "option token '"+token+"' does not start with dash" );
				
				// short option (e.g., -c)
				
				String c = token.substring( 1 );

				// rule out any but single char
				if (c.length() != 1)
					throw new IllegalArgumentException( "short option token '"+token+"' must have only one char after the dash" );

				checkValidChars( c, "short option", token, shortTokenChars );
				defineOption( shortOptions, c, option );
			}
		}
		
		optionList.add( option );
	}
	
	private void checkValidChars( String name, String descr, String token, Pattern pattern )
	{
		if (!pattern.matcher( name ).matches())
			throw new IllegalArgumentException( descr+" token '"+token+"' has bad chars" );
	}
	
	private final Pattern longTokenChars = Pattern.compile( "^[a-z0-9]+(-[a-z0-9]+)*$" );
	
	private final Pattern shortTokenChars = Pattern.compile( "^[a-zA-Z0-9]$" );
	
	private void defineOption( Map<String, Option> map, String token, Option option )
	{
		Option other = map.put( token, option );
		if (other != null)
			throw new IllegalArgumentException(
				"options '"+option+"' and '"+other+"' have overlapping token sets" );
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
			
			if (token.startsWith( "-" ))
			{
				// might be a long option (e.g., --name), a short option
				// (e.g., -c) or a parameter (-).
				if (token.startsWith( "--" ))
				{
					// handle long option (e.g., --name)
					
					String[] x = StringUtil.leftSplit( token, '=' );
					String tok = null;
					String val = null;
					if (x != null)
					{
						tok = x[0];
						val = x[1];
					}
					else
					{
						tok = token;
						val = null;
					}
					
					Option option = findOption( tok.substring( 2 ) );
					if (option == null)
					{
						report( "unexpected command line option", token, argsIterator );
						return false;
					}
					
					Object[] values = null;
					
					if (option.wantsValue())
					{
						values = option.getAndConvertValue( tok, val, argsIterator );
						if (values == null)
							return false;
						
						if (!option.checkValue( tok, values ))
							return false;
					}
					else
					{
						if (val != null)
							report( "unexpected option value", tok, argsIterator );
					}
					
					if (!option.deliverValue( tok, values, false ))
						return false;
					
					continue;
				}
				else if (token.length() > 1)
				{
					// handle short option (e.g., -c)
					
					CharIterator i = new CharIterator( token.substring( 1 ) );
					while (i.hasNext())
					{
						String tok = String.valueOf( i.next() );
						
						Option option = findShortOption( tok );
						if (option == null)
						{
							report( String.format(
								"unexpected command line option (-%s)", tok ),
								token, argsIterator );
							return false;
						}
						
						Object[] values = null;
						
						if (option.wantsValue())
						{
							String val = rest( i );
							
							values = option.getAndConvertValue( tok, val, argsIterator );
							
							if (values == null)
								return false;
							
							if (!option.checkValue( tok, values ))
								return false;
						}
						
						if (!option.deliverValue( tok, values, false ))
							return false;
					}
					continue;
				}
				// token is -, which is a parameter, so just fall through...
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
	
	private String rest( CharIterator i )
	{
		if (!i.hasNext())
			return null;
		
		StringBuffer sb = new StringBuffer();
		while (i.hasNext())
			sb.append( i.next() );
		return sb.toString();
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
		Option o = getOption( token );
//		System.out.printf( "findOption( %s ) found %s\n", token, o );
		return o;
	}

	/**
	 * @param token
	 * @return the option matching token or null.
	 */
	private Option findShortOption( String token )
	{
		Option o = getShortOption( token );
//		System.out.printf( "findShortOption( %s ) found %s\n", token, o );
		return o;
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
	
	private final Map<String,Option> options = new HashMap<String, Option>();
	
	private final Map<String,Option> shortOptions = new HashMap<String, Option>();
	
	private final List<Option> optionList = new ArrayList<Option>();
	
	private final List<Parameter> parameters = new ArrayList<Parameter>();
	
	private Parameter catchAllParameter;

	/**
	 * @param justUsage true if the
	 */
	public void showHelp( boolean justUsage )
	{
		System.err.print( "usage: " );
		
		System.err.print( program );
		
		boolean hasOptions = false;
		
		{
			Iterator<Option> i = getOptionList();
			while (i.hasNext())
			{
				Option option = i.next();
				if (option.isHidden())
					continue;
				
				// only show [options] if there are any visible options...
				System.err.print( " [options]" );
				hasOptions = true;
				break;
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
				if (hasOptions)
				{
					System.err.println();
					System.err.println( "OPTIONS:" );
				}
				
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
				
				if (i.hasNext() || catchAllParameter != null)
				{
					System.err.println();
					System.err.println( "PARAMETERS:" );
				}
				
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

	/**
	 * @param token
	 * @return the option for the specified token, or null.
	 */
	public Option getShortOption( String token )
	{
		return shortOptions.get( token );
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
	 * Defines a boolean option. The value of the boolean option may be
	 * specified as true, false, yes, no, 1, 0, enable, or disable (e.g., -x0
	 * would specify the boolean option -x as false).
	 * 
	 * @param tokens a sequence of short and long tokens which are used to
	 * specify the option (e.g., "-x|--extend"). The tokens are separated from
	 * each other by vertical bar, comma, semi-colon, or any sort of whitespace.
	 * The tokens come in short (-x) and long (--extend) varieties. Short tokens
	 * may only have a single alphanumeric character after the dash. Case is
	 * significant. Long tokens must match this pattern: [a-z0-9]+(-[a-z0-9]+)*.
	 * That is, one or more alphanumeric characters broken up by single internal
	 * dashes. No upper case. Note that -x and --x are not the same.
	 * 
	 * @param name the name of the value of the option.
	 * 
	 * @param method the name of a method of the target class to call with the
	 * option when it is specified or defaulted. The method must have the
	 * signature boolean method(CommandParser cp, Option option, String token,
	 * Boolean value) or void method(CommandParser cp, Option option, String
	 * token, Boolean value).
	 * 
	 * @param description a description of the option which is formatted into a
	 * help message for the command.
	 * 
	 * @param flags some flag values which may be or'ed together.
	 * 
	 * @param defaultValue a default value for the option.
	 * 
	 * @param constraint a constraint expression for values of the option.
	 * 
	 * @throws Exception if there are problems with the tokens, the method, or
	 * the flags.
	 * 
	 * @see Option#HIDDEN
	 * @see Option#NONE
	 * @see Option#REQUIRED
	 * @see Option#SINGLETON
	 */
	public void defineBooleanOption( String tokens, String name, String method,
		String description, int flags, Boolean defaultValue, Constraint constraint )
		throws Exception
	{
		define( new BooleanOption( this, tokens, name, method, description, flags,
			defaultValue, constraint ) );
	}

	/**
	 * Defines a double option. The value of the double option may be specified
	 * as any valid floating point number (e.g., -x0.1 would specify the double
	 * option -x as 0.1).
	 * 
	 * @param tokens a sequence of short and long tokens which are used to
	 * specify the option (e.g., "-x|--extend"). The tokens are separated from
	 * each other by vertical bar, comma, semi-colon, or any sort of whitespace.
	 * The tokens come in short (-x) and long (--extend) varieties. Short tokens
	 * may only have a single alphanumeric character after the dash. Case is
	 * significant. Long tokens must match this pattern: [a-z0-9]+(-[a-z0-9]+)*.
	 * That is, one or more alphanumeric characters broken up by single internal
	 * dashes. No upper case. Note that -x and --x are not the same.
	 * 
	 * @param name the name of the value of the option.
	 * 
	 * @param method the name of a method of the target class to call with the
	 * option when it is specified or defaulted. The method must have the
	 * signature boolean method(CommandParser cp, Option option, String token,
	 * Double value) or void method(CommandParser cp, Option option, String
	 * token, Double value).
	 * 
	 * @param description a description of the option which is formatted into a
	 * help message for the command.
	 * 
	 * @param flags some flag values which may be or'ed together.
	 * 
	 * @param defaultValue a default value for the option.
	 * 
	 * @param constraint a constraint expression for values of the option.
	 * 
	 * @throws Exception if there are problems with the tokens, the method, or
	 * the flags.
	 * 
	 * @see Option#HIDDEN
	 * @see Option#NONE
	 * @see Option#REQUIRED
	 * @see Option#SINGLETON
	 */
	public void defineDoubleOption( String tokens, String name, String method,
		String description, int flags, Double defaultValue, Constraint constraint )
		throws Exception
	{
		define( new DoubleOption( this, tokens, name, method, description, flags,
			defaultValue, constraint ) );
	}

	/**
	 * Defines a file option. The value of the file option may be specified
	 * as any valid file or directory name (e.g., -xfoo.txt would specify the
	 * file option -x as foo.txt).
	 * 
	 * @param tokens a sequence of short and long tokens which are used to
	 * specify the option (e.g., "-x|--extend"). The tokens are separated from
	 * each other by vertical bar, comma, semi-colon, or any sort of whitespace.
	 * The tokens come in short (-x) and long (--extend) varieties. Short tokens
	 * may only have a single alphanumeric character after the dash. Case is
	 * significant. Long tokens must match this pattern: [a-z0-9]+(-[a-z0-9]+)*.
	 * That is, one or more alphanumeric characters broken up by single internal
	 * dashes. No upper case. Note that -x and --x are not the same.
	 * 
	 * @param name the name of the value of the option.
	 * 
	 * @param method the name of a method of the target class to call with the
	 * option when it is specified or defaulted. The method must have the
	 * signature boolean method(CommandParser cp, Option option, String token,
	 * File value) or void method(CommandParser cp, Option option, String
	 * token, File value).
	 * 
	 * @param description a description of the option which is formatted into a
	 * help message for the command.
	 * 
	 * @param flags some flag values which may be or'ed together.
	 * 
	 * @param defaultValue a default value for the option.
	 * 
	 * @param constraint a constraint expression for values of the option.
	 * 
	 * @throws Exception if there are problems with the tokens, the method, or
	 * the flags.
	 * 
	 * @see Option#HIDDEN
	 * @see Option#NONE
	 * @see Option#REQUIRED
	 * @see Option#SINGLETON
	 */
	public void defineFileOption( String tokens, String name, String method,
		String description, int flags, File defaultValue, Constraint constraint )
		throws Exception
	{
		define( new FileOption( this, tokens, name, method, description, flags,
			defaultValue, constraint ) );
	}

	/**
	 * Defines an integer option. The value of the integer option may be specified
	 * as any valid integral number (e.g., -x123 would specify the integer
	 * option -x as 123).
	 * 
	 * @param tokens a sequence of short and long tokens which are used to
	 * specify the option (e.g., "-x|--extend"). The tokens are separated from
	 * each other by vertical bar, comma, semi-colon, or any sort of whitespace.
	 * The tokens come in short (-x) and long (--extend) varieties. Short tokens
	 * may only have a single alphanumeric character after the dash. Case is
	 * significant. Long tokens must match this pattern: [a-z0-9]+(-[a-z0-9]+)*.
	 * That is, one or more alphanumeric characters broken up by single internal
	 * dashes. No upper case. Note that -x and --x are not the same.
	 * 
	 * @param name the name of the value of the option.
	 * 
	 * @param method the name of a method of the target class to call with the
	 * option when it is specified or defaulted. The method must have the
	 * signature boolean method(CommandParser cp, Option option, String token,
	 * Integer value) or void method(CommandParser cp, Option option, String
	 * token, Integer value).
	 * 
	 * @param description a description of the option which is formatted into a
	 * help message for the command.
	 * 
	 * @param flags some flag values which may be or'ed together.
	 * 
	 * @param defaultValue a default value for the option.
	 * 
	 * @param constraint a constraint expression for values of the option.
	 * 
	 * @throws Exception if there are problems with the tokens, the method, or
	 * the flags.
	 * 
	 * @see Option#HIDDEN
	 * @see Option#NONE
	 * @see Option#REQUIRED
	 * @see Option#SINGLETON
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
	 * Defines a string option. The value of the string option may be specified
	 * as any valid characters (e.g., -xxyz would specify the string
	 * option -x as xyz).
	 * 
	 * @param tokens a sequence of short and long tokens which are used to
	 * specify the option (e.g., "-x|--extend"). The tokens are separated from
	 * each other by vertical bar, comma, semi-colon, or any sort of whitespace.
	 * The tokens come in short (-x) and long (--extend) varieties. Short tokens
	 * may only have a single alphanumeric character after the dash. Case is
	 * significant. Long tokens must match this pattern: [a-z0-9]+(-[a-z0-9]+)*.
	 * That is, one or more alphanumeric characters broken up by single internal
	 * dashes. No upper case. Note that -x and --x are not the same.
	 * 
	 * @param name the name of the value of the option.
	 * 
	 * @param method the name of a method of the target class to call with the
	 * option when it is specified or defaulted. The method must have the
	 * signature boolean method(CommandParser cp, Option option, String token,
	 * String value) or void method(CommandParser cp, Option option, String
	 * token, String value).
	 * 
	 * @param description a description of the option which is formatted into a
	 * help message for the command.
	 * 
	 * @param flags some flag values which may be or'ed together.
	 * 
	 * @param defaultValue a default value for the option.
	 * 
	 * @param constraint a constraint expression for values of the option.
	 * 
	 * @throws Exception if there are problems with the tokens, the method, or
	 * the flags.
	 * 
	 * @see Option#HIDDEN
	 * @see Option#NONE
	 * @see Option#REQUIRED
	 * @see Option#SINGLETON
	 */
	public void defineStringOption( String tokens, String name, String method,
		String description, int flags, String defaultValue, Constraint constraint )
		throws Exception
	{
		define( new StringOption( this, tokens, name, method, description,
			flags, defaultValue, constraint ) );
	}

	/**
	 * Defines a mysterious thing.
	 * 
	 * @param tokens a sequence of short and long tokens which are used to
	 * specify the option (e.g., "-x|--extend"). The tokens are separated from
	 * each other by vertical bar, comma, semi-colon, or any sort of whitespace.
	 * The tokens come in short (-x) and long (--extend) varieties. Short tokens
	 * may only have a single alphanumeric character after the dash. Case is
	 * significant. Long tokens must match this pattern: [a-z0-9]+(-[a-z0-9]+)*.
	 * That is, one or more alphanumeric characters broken up by single internal
	 * dashes. No upper case. Note that -x and --x are not the same.
	 * 
	 * @param names the names of the values of the option.
	 * 
	 * @param method the name of a method of the target class to call with the
	 * option when it is specified or defaulted. The method must have the
	 * signature boolean method(CommandParser cp, Option option, String token,
	 * Object[] values) or void method(CommandParser cp, Option option, String
	 * token, Object[] value).
	 * 
	 * @param description a description of the option which is formatted into a
	 * help message for the command.
	 * 
	 * @param flags some flag values which may be or'ed together.
	 * 
	 * @param defaultValues default values for the option.
	 * 
	 * @param constraints constraint expressions for values of the option.
	 * 
	 * @throws Exception if there are problems with the tokens, the method, or
	 * the flags.
	 * 
	 * @see Option#HIDDEN
	 * @see Option#NONE
	 * @see Option#REQUIRED
	 * @see Option#SINGLETON
	 */
	public void defineStringArrayOption( String tokens, String[] names,
		String method, String description, int flags, String[] defaultValues,
		Constraint[] constraints ) throws Exception
	{
		define( new StringArrayOption( this, tokens, names, method,
			description, flags, defaultValues, constraints ) );
	}

	/**
	 * Defines a url option. The value of the url option may be specified
	 * as any valid url (e.g., -xhttp://foo.com would specify the url
	 * option -x as http://foo.com).
	 * 
	 * @param tokens a sequence of short and long tokens which are used to
	 * specify the option (e.g., "-x|--extend"). The tokens are separated from
	 * each other by vertical bar, comma, semi-colon, or any sort of whitespace.
	 * The tokens come in short (-x) and long (--extend) varieties. Short tokens
	 * may only have a single alphanumeric character after the dash. Case is
	 * significant. Long tokens must match this pattern: [a-z0-9]+(-[a-z0-9]+)*.
	 * That is, one or more alphanumeric characters broken up by single internal
	 * dashes. No upper case. Note that -x and --x are not the same.
	 * 
	 * @param name the name of the value of the option.
	 * 
	 * @param method the name of a method of the target class to call with the
	 * option when it is specified or defaulted. The method must have the
	 * signature boolean method(CommandParser cp, Option option, String token,
	 * URL value) or void method(CommandParser cp, Option option, String
	 * token, URL value).
	 * 
	 * @param description a description of the option which is formatted into a
	 * help message for the command.
	 * 
	 * @param flags some flag values which may be or'ed together.
	 * 
	 * @param defaultValue a default value for the option.
	 * 
	 * @param constraint a constraint expression for values of the option.
	 * 
	 * @throws Exception if there are problems with the tokens, the method, or
	 * the flags.
	 * 
	 * @see Option#HIDDEN
	 * @see Option#NONE
	 * @see Option#REQUIRED
	 * @see Option#SINGLETON
	 */
	public void defineUrlOption( String tokens, String name, String method,
		String description, int flags, URL defaultValue, Constraint constraint )
		throws Exception
	{
		define( new UrlOption( this, tokens, name, method, description, flags,
			defaultValue, constraint ) );
	}
}
