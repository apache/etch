/* $Id: Option.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Mar 27, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.cmd;

import java.lang.reflect.InvocationTargetException;
import java.util.Iterator;
import java.util.List;
import java.util.StringTokenizer;
import java.util.Vector;

/**
 * A command line value which is introduced by a specified token.
 * An option is not dependent upon position on the command line
 * to be recognised (but still, position on the command line might
 * be important).
 */
abstract public class Option extends OptParamBase
{
	/**
	 * @param cp
	 * @param tokens
	 * @param name
	 * @param method
	 * @param params
	 * @param description
	 * @param flags
	 * @param defaultValue
	 * @param constraint
	 * @throws Exception
	 */
	public Option( CommandParser cp, String tokens, String name, String method,
		Class<?>[] params, String description, int flags, Object defaultValue,
		Constraint constraint ) throws Exception
	{
		super( cp, name, method, params, description, (flags & REQUIRED) != 0,
			constraint );
		
		if (!setTokens( tokens ))
			throw new IllegalArgumentException( "no tokens for option" );
		
		this.isArray = params.length > 3 && params[3].isArray();
		this.flags = flags;
		
		if (defaultValue != null)
			this.defaultValues = new Object[] { defaultValue };
		else
			this.defaultValues = null;
		
		if (constraint != null && defaultValue != null)
			constraint.checkValue( defaultValue );
	}
	/**
	 * @param cp
	 * @param tokens
	 * @param names
	 * @param method
	 * @param params
	 * @param description
	 * @param flags
	 * @param defaultValues
	 * @param constraints
	 * @throws Exception
	 */
	public Option( CommandParser cp, String tokens, String[] names, String method,
		Class<?>[] params, String description, int flags, Object[] defaultValues,
		Constraint[] constraints ) throws Exception
	{
		super( cp, names, method, params, description, (flags & REQUIRED) != 0,
			constraints );
		
		if (!setTokens( tokens ))
			throw new IllegalArgumentException( "no tokens for option" );
		
		if (defaultValues != null && defaultValues.length != names.length)
			throw new IllegalArgumentException( "defaultValue.length != name.length" );
		
		this.isArray = params.length > 3 && params[3].isArray();
		this.flags = flags;
		this.defaultValues = defaultValues;
		
		if (constraints != null && defaultValues != null)
		{
			int n = constraints.length;
			for (int i = 0; i < n; i++)
			{
				Constraint c = constraints[i];
				Object v = defaultValues[i];
				if (c != null)
					c.checkValue( v );
			}
		}
	}

	private final boolean isArray;
	
	private final int flags;
	
	private final Object[] defaultValues;
	
	/**
	 * The option has no special handling.
	 */
	public static final int NONE = 0;

	/**
	 * The option is hidden from the user. It must not be specified.
	 * It must not be required or it must have a default value.
	 */
	public static final int HIDDEN = 1;

	/**
	 * The option may only be specified once.
	 */
	public static final int SINGLETON = 2;

	/**
	 * The option is required to be specified.
	 */
	public static final int REQUIRED = 4;
	
	/**
	 * @return an iterator over the tokens of an option.
	 */
	public Iterator<String> getTokens()
	{
		return tokens.iterator();
	}
	
	/**
	 * @return true if the option is hidden.
	 */
	public boolean isHidden()
	{
		return matchFlags( HIDDEN );
	}
	
	/**
	 * @return true if the option may only appear once.
	 */
	public boolean isSingleton()
	{
		return matchFlags( SINGLETON );
	}

	/**
	 * @param flag
	 * @return true if the flag has been specified, false otherwise
	 */
	private boolean matchFlags( int flag )
	{
		return (flags & flag) != 0;
	}
	
	/**
	 * @return the default value of the option.
	 */
	public Object[] getDefaultValues()
	{
		return defaultValues;
	}

	@Override
	public String toString()
	{
		return toString( primaryToken );
	}
	
	/**
	 * @param token
	 * @return the 
	 */
	protected String toString( String token )
	{
		StringBuffer sb = new StringBuffer( token );
		
		for (String name: getNames())
		{
			sb.append( ' ' );
			sb.append( name );
		}
		
		return sb.toString();
	}

	////////////
	// TOKENS //
	////////////

	private boolean setTokens( String s )
	{
		tokens = new Vector<String>();
		primaryToken = null;
		
		StringTokenizer st = new StringTokenizer( s, "|,; \r\n\t" );
		while (st.hasMoreTokens())
		{
			String t = st.nextToken();
			tokens.add( t );
			if (primaryToken == null)
				primaryToken = t;
		}
		
		return tokens.size() > 0;
	}
	
	/**
	 * @return the first token in the token list of the option.
	 */
	public String getPrimaryToken()
	{
		return primaryToken;
	}
	
	private List<String> tokens;
	
	private String primaryToken;

	//////////
	// HELP //
	//////////

	@Override
	public void showShortDescription()
	{
		System.err.print( isRequired() ? " {" : " [" );
		
		System.err.print( ' ' );
		System.err.print( primaryToken );
		
		if (wantsValue())
		{
			for (String name: getNames())
			{
				System.err.print( ' ' );
				System.err.print( name );
			}
		}
		
		System.err.print( isRequired() ? " }" : " ]" );
	}
	
	@Override
	public void showLongDescription()
	{
		Iterator<String> i = tokens.iterator();
		while (i.hasNext())
		{
			System.err.print( NAME_FLAG );
			System.err.print( i.next() );
			if (wantsValue())
			{
				for (String name: getNames())
				{
					System.err.print( ' ' );
					System.err.print( name );
				}
			}
			System.err.println();
		}
		
		showDescription();
		
		showIsRequired();
		
		showConstraint();
		
		if (isSingleton())
		{
			System.err.print( DESC_FLAG );
			System.err.println( "singleton (may only be specified once)." );
		}
		
		if (defaultValues != null)
		{
			int n = defaultValues.length;
			
			System.err.print( DESC_FLAG );
			
			if (n > 1)
				System.err.print( "default: {" );
			else
				System.err.print( "default: " );
			
			for (Object defaultValue: defaultValues)
			{
				System.err.print( ' ' );
				System.err.print( defaultValue );
			}
			
			if (n > 1)
				System.err.println( " }" );
			else
				System.err.println();
		}
	}

	///////////
	// VALUE //
	///////////
	
	/**
	 * @return true if this option needs a value, false otherwise.
	 */
	public boolean wantsValue()
	{
		return true;
	}

	/**
	 * @param token
	 * @param tkns
	 * @return value from tokens converted as appropriate.
	 */
	public Object[] getAndConvertValue( String token, Iterator<String> tkns )
	{
		Object[] values = new Object[getNames().length];
		int i = 0;
		for (String name: getNames())
		{
			if (!tkns.hasNext())
			{
				System.err.println( "option '"+toString( token )+"': missing value "+name );
				return null;
			}
			values[i++] = convertValue( tkns.next() );
		}
		return values;
	}
	
	@Override
	public boolean checkValue( String token, Object[] values )
	{
		return super.checkValue( "option '"+toString( token )+"'", values );
	}

	/**
	 * @param token
	 * @param values
	 * @param hiddenOk
	 * @return value returned by called method.
	 * @throws Exception
	 */
	public boolean deliverValue( String token, Object[] values, boolean hiddenOk )
		throws Exception
	{
		try
		{
			if (!checkOkToDeliver( token, hiddenOk ))
				return false;
			
			if (isArray)
				return callMethod( getArgs( token, values ) );
			
			return callMethod( getArgs( token, values != null ? values[0] : null ) );
		}
		catch ( InvocationTargetException e )
		{
			throw (Exception) e.getTargetException();
		}
	}

	/**
	 * @param token
	 * @param hiddenOk
	 * @return false if flags prevent delivery of the option value.
	 */
	private boolean checkOkToDeliver( String token, boolean hiddenOk )
	{
		if (isHidden() && !hiddenOk)
		{
			getCommandParser().report( "option '"+toString( token )+"' not allowed", null, null );
			return false;
		}
		
		if (getCommandParser().isAlreadySpecified( this, true ) && isSingleton())
		{
			getCommandParser().report( "option '"+toString( token )+"' not allowed more than once", null, null );
			return false;
		}
		
		return true;
	}

	/**
	 * @param token
	 * @param value
	 * @return the args to pass to the method.
	 */
	protected Object[] getArgs( String token, Object value )
	{
		return new Object[] { getCommandParser(), this, token, value };
	}
}
