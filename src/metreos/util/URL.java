/* $Id: URL.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Feb 16, 2005
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.util;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

/**
 * Models a url of the form scheme://user:password@host:port/uri;parms?terms#fragment
 */
public class URL
{
	/**
	 * Constructs a url from a string.
	 * @param urlStr
	 */
	public URL( String urlStr )
	{
		parse( urlStr );
	}
	
	/**
	 * Constructs a url from another url.
	 * @param other
	 */
	public URL( URL other )
	{
		this.scheme = other.scheme;
		this.user = other.user;
		this.password = other.password;
		this.host = other.host;
		this.port = other.port;
		this.uri = other.uri;
		this.params = copyParams( other.params );
		this.terms = copyTerms( other.terms );
		this.fragment = other.fragment;
	}

	/**
	 * Constructs an empty url.
	 */
	public URL()
	{
		// nothing to do.
	}
	
	private void parse( String s )
	{
		// s is scheme:[//[user[:password]@]host[:port]/]uri[;params][?terms][#fragment]
		
		String[] x = StringUtil.leftSplit( s, ':' );
		if (x == null)
			throw new IllegalArgumentException( "missing scheme" );
		
		setScheme( unescape( x[0] ) );
		s = x[1];
		
		// s is [//[user[:password]@]host[:port]/]uri[;params][?terms][#fragment]
		
		x = StringUtil.leftSplit( s, '#' );
		if (x != null)
		{
			setFragment( unescape( x[1] ) );
			s = x[0];
		}
		
		// s is [//[user[:password]@]host[:port]/]uri[;params][?terms]
		
		x = StringUtil.leftSplit( s, '?' );
		if (x != null)
		{
			parseTerms( x[1] );
			s = x[0];
		}
		
		// s is [//[user[:password]@]host[:port]/]uri[;params]
		
		x = StringUtil.leftSplit( s, ';' );
		if (x != null)
		{
			parseParams( x[1] );
			s = x[0];
		}
		
		// s is [//[user[:password]@]host[:port]/]uri
		
		if (s.startsWith( "//" ))
		{
			s = s.substring( 2 );
			// s is [user[:password]@]host[:port]/uri
			x = StringUtil.leftSplit( s, '/' );
			if (x != null)
			{
				// s is [user[:password]@]host[:port]/uri
				parseHost( x[0] );
				s = x[1];
			}
			else
			{
				// s is [user[:password]@]host[:port]
				parseHost( s );
				s = "";
			}
		}
		setUri( unescape( s ) );
	}

	private void parseHost( String s )
	{
		// s is [user[:password]@]host[:port]
		String[] x = StringUtil.leftSplit( s, '@' );
		if (x != null)
		{
			parseUserPassword( x[0] );
			parseHostPort( x[1] );
		}
		else
		{
			parseHostPort( s );
		}
	}

	private void parseUserPassword( String s )
	{
		// s is user[:password]
		String[] x = StringUtil.leftSplit( s, ':' );
		if (x != null)
		{
			setUser( unescape( x[0] ) );
			setPassword( unescape( x[1] ) );
		}
		else
		{
			setUser( unescape( s ) );
		}
	}

	private void parseHostPort( String s )
	{
		// s is host[:port]
		String[] x = StringUtil.leftSplit( s, ':' );
		if (x != null)
		{
			setHost( unescape( x[0] ) );
			setPort( Integer.parseInt( unescape( x[1] ) ) );
		}
		else
		{
			setHost( unescape( s ) );
		}
	}

	private void parseParams( String s )
	{
		// s is param[;param]*
		
		if (s.length() == 0)
			return;
		
		ensureParams();
		
		String[] x;
		while ((x = StringUtil.leftSplit( s, ';' )) != null)
		{
			addParam( unescape( x[0] ) );
			s = x[1];
		}
		
		addParam( unescape( s ) );
	}

	private void parseTerms( String s )
	{
		// s is term[&term]*
		
		if (s.length() == 0)
			return;
		
		String[] x;
		while ((x = StringUtil.leftSplit( s, '&' )) != null)
		{
			parseTerm( x[0] );
			s = x[1];
		}
		
		parseTerm( s );
	}

	private void parseTerm( String s )
	{
		// s is name[=value]
		
		if (s.length() == 0)
			return;
		
		ensureTerms();
		
		String[] x = StringUtil.leftSplit( s, '=' );
		if (x != null)
			addTerm( unescape( x[0] ), unescape( x[1] ) );
		else
			addTerm( unescape( s ), "" );
	}

	private String scheme;
	
	private String user;
	
	private String password;
	
	private String host;
	
	private int port;
	
	private String uri;
	
	private Set<String> params;
	
	private Map<String,Object> terms;
	
	private String fragment;

	////////////
	// SCHEME //
	////////////
	
	/**
	 * @return the scheme.
	 */
	public String getScheme()
	{
		return scheme;
	}
	
	/**
	 * Sets the scheme.
	 * 
	 * @param scheme the scheme to set which may be null but not blank.
	 */
	public void setScheme( String scheme )
	{
		checkNotBlank( "scheme", scheme );
		this.scheme = scheme;
	}
	
	/**
	 * Tests the scheme for a match. The schemes are case-insensitive.
	 * 
	 * @param testScheme a scheme to test against.
	 * 
	 * @return true if the schemes match, false otherwise.
	 */
	public boolean isScheme( String testScheme )
	{
		return testScheme.equalsIgnoreCase( scheme );
	}

	//////////
	// USER //
	//////////
	
	/**
	 * @return the user.
	 */
	public String getUser()
	{
		return user;
	}
	
	/**
	 * Sets the user.
	 * 
	 * @param user the user to set which may be null but not blank.
	 */
	public void setUser( String user )
	{
		checkNotBlank( "user", user );
		this.user = user;
	}

	//////////////
	// PASSWORD //
	//////////////
	
	/**
	 * @return the password.
	 */
	public String getPassword()
	{
		return password;
	}
	
	/**
	 * Sets the password.
	 * 
	 * @param password the password to set which may be null but not blank.
	 */
	public void setPassword( String password )
	{
		checkNotBlank( "password", password );
		this.password = password;
	}

	//////////
	// HOST //
	//////////

	/**
	 * @return the host.
	 */
	public String getHost()
	{
		return host;
	}

	/**
	 * Sets the host.
	 * 
	 * @param host the host to set which may be null but not blank.
	 */
	public void setHost( String host )
	{
		checkNotBlank( "host", host );
		this.host = host;
	}

	//////////
	// PORT //
	//////////

	/**
	 * @return the port.
	 */
	public int getPort()
	{
		return port;
	}

	/**
	 * Sets the port.
	 * 
	 * @param port the port to set which may be between 0 and 65535.
	 */
	public void setPort( int port )
	{
		if (port < 0 || port > 65535)
			throw new IllegalArgumentException( "port < 0 || port > 65535" );
		this.port = port;
	}

	/////////
	// URI //
	/////////
	
	/**
	 * @return the uri.
	 */
	public String getUri()
	{
		return uri;
	}

	/**
	 * Sets the uri.
	 * 
	 * @param uri the uri to set which may be null or blank.
	 */
	public void setUri( String uri )
	{
//		checkNotBlank( "uri", uri );
		this.uri = uri;
	}

	////////////
	// PARAMS //
	////////////
	
	/**
	 * @return true if there is at least one param.
	 */
	public boolean hasParams()
	{
		return params != null && params.size() > 0;
	}

	/**
	 * Fetchs the first param found which starts with the given prefix. The
	 * search order is not specified, and the params are not maintained in any
	 * specific order.
	 * 
	 * @param prefix the prefix of the param to fetch (e.g., "transport=").
	 * 
	 * @return the param which starts with the specified prefix.
	 */
	public String getParam( String prefix )
	{
		if (params == null)
			return null;
		
		Iterator<String> i = getParams();
		while (i.hasNext())
		{
			String s = i.next();
			if (s.startsWith( prefix ))
				return s;
		}
		return null;
	}
	
	/**
	 * @return an iterator over all the params. The params are strings, generally
	 * of the form "transport=tcp". But they can be anything you like, really.
	 * The iterator might be empty.
	 */
	public Iterator<String> getParams()
	{
		if (params == null)
			return new EmptyIterator<String>();
		
		return params.iterator();
	}
	
	/**
	 * Adds a param to the set of params for this url. Only the set of unique
	 * params is maintained. Duplicate param values are suppressed.
	 * 
	 * @param param a param (e.g., "transport=tcp" or "01831864574898475").
	 */
	public void addParam( String param )
	{
		ensureParams();
		params.add( param );
	}
	
	/**
	 * Removes the first param found which starts with the given prefix. The
	 * search order is not specified, and the params are not maintained in any
	 * specific order.
	 * 
	 * @param prefix the prefix of the param to remove (e.g., "transport=").
	 * 
	 * @return the param removed.
	 */
	public String removeParam( String prefix )
	{
		if (params == null)
			return null;
		
		Iterator<String> i = getParams();
		while (i.hasNext())
		{
			String s = i.next();
			if (s.startsWith( prefix ))
			{
				i.remove();
				return s;
			}
		}
		return null;
	}
	
	/**
	 * Removes all the params.
	 */
	public void clearParams()
	{
		if (params != null)
			params.clear();
	}
	
	private void ensureParams()
	{
		if (params == null)
			params = new HashSet<String>();
	}

	/////////////////
	// QUERY TERMS //
	/////////////////
	
	/**
	 * @return true if there is at least one query term. Query terms
	 * are of the form name=value.
	 */
	public boolean hasTerms()
	{
		return terms != null && terms.size() > 0;
	}
	
	/**
	 * @param name
	 * 
	 * @return true if there is at least one query term with the specified
	 * name.
	 */
	public boolean hasTerm( String name )
	{
		if (terms == null)
			return false;
		
		return terms.containsKey( name );
	}
	
	/**
	 * @param name
	 * @param value
	 * @return true if there is a query term with the specified value.
	 */
	public boolean hasTerm( String name, String value )
	{
		if (terms == null)
			return false;
		
		Object obj = terms.get( name );
		if (obj == null)
			return false;
		
		if (obj instanceof Set)
			return ((Set) obj).contains( value );
		
		return obj.equals( value );
	}

	/**
	 * @param name
	 * @param value
	 * @return true if there is a query term with the specified value.
	 */
	public boolean hasTerm( String name, Integer value )
	{
		return hasTerm( name, value.toString() );
	}

	/**
	 * @param name
	 * @param value
	 * @return true if there is a query term with the specified value.
	 */
	public boolean hasTerm( String name, Double value )
	{
		return hasTerm( name, value.toString() );
	}

	/**
	 * @param name
	 * @param value
	 * @return true if there is a query term with the specified value.
	 */
	public boolean hasTerm( String name, int value )
	{
		return hasTerm( name, valueToString( value ) );
	}

	/**
	 * @param name
	 * @param value
	 * @return true if there is a query term with the specified value.
	 */
	public boolean hasTerm( String name, double value )
	{
		return hasTerm( name, valueToString( value ) );
	}
	
	/**
	 * @param name
	 * @return true if the query term specified by name has multiple values.
	 */
	public boolean hasMultipleValues( String name )
	{
		if (terms == null)
			return false;
		
		Object obj = terms.get( name );
		if (obj == null)
			return false;
		
		if (obj instanceof Set)
			return ((Set) obj).size() > 1;
		
		return false;
	}
	
	/**
	 * Gets the value of the specified query term.
	 * 
	 * @param name
	 * 
	 * @return the value of the specified term, or null if not found.
	 * 
	 * @throws UnsupportedOperationException if the query term has multiple values.
	 * Use getTerms(name) instead.
	 * 
	 * @see #getTerms(String)
	 * @see #getIntegerTerm(String)
	 * @see #getDoubleTerm(String)
	 */
	@SuppressWarnings(value={"unchecked"})
	public String getTerm( String name )
	{
		if (terms == null)
			return null;
		
		Object obj = terms.get( name );
		if (obj == null)
			return null;
		
		if (obj instanceof Set)
		{
			Iterator<String> i = ((Set<String>) obj).iterator();
			String s = i.next();
			if (i.hasNext())
				throw new UnsupportedOperationException( "term has multiple values" );
			return s;
		}
		
		return (String) obj;
	}

	/**
	 * @param name
	 * @param defaultValue
	 * @return the value of the specified term, or defaultValue if not found.
	 */
	public String getTerm( String name, String defaultValue )
	{
		String value = getTerm( name );
		if (value == null)
			return defaultValue;
		return value;
	}

	/**
	 * Gets the boolean value of the specified query term.
	 * @param name
	 * @return the boolean value, or null if not found.
	 * @see #getTerm(String)
	 */
	public Boolean getBooleanTerm( String name )
	{
		String s = getTerm( name );
		if (s == null)
			return null;
		return new Boolean( s );
	}

	/**
	 * Gets the boolean value of the specified query term.
	 * @param name
	 * @param defaultValue the value to return if the term is not found.
	 * @return the boolean value, or defaultValue if not found.
	 * @see #getTerm(String)
	 */
	public boolean getBooleanTerm( String name, boolean defaultValue )
	{
		String s = getTerm( name );
		if (s == null)
			return defaultValue;
		return Boolean.valueOf( s );
	}
	
	/**
	 * Gets the integer value of the specified query term.
	 * @param name
	 * @return the integer value, or null if not found.
	 * @see #getTerm(String)
	 */
	public Integer getIntegerTerm( String name )
	{
		String s = getTerm( name );
		if (s == null)
			return null;
		return new Integer( s );
	}

	/**
	 * Gets the integer value of the specified query term.
	 * @param name
	 * @param defaultValue the value to return if the term is not found.
	 * @return the integer value, or defaultValue if not found.
	 * @see #getTerm(String)
	 */
	public int getIntegerTerm( String name, int defaultValue )
	{
		Integer i = getIntegerTerm( name );
		if (i == null)
			return defaultValue;
		return i;
	}
	
	/**
	 * Gets the double value of the specified query term.
	 * @param name
	 * @return the double value, or null if not found.
	 * @see #getTerm(String)
	 */
	public Double getDoubleTerm( String name )
	{
		String s = getTerm( name );
		if (s == null)
			return null;
		return new Double( s );
	}
	
	/**
	 * Gets the values of the specified query term.
	 * @param name
	 * @return an iterator over the string values of the query term. May be empty.
	 */
	@SuppressWarnings(value={"unchecked"})
	public Iterator<String> getTerms( String name )
	{
		if (terms == null)
			return null;
		
		Object obj = terms.get( name );
		if (obj == null)
			return new EmptyIterator<String>();
		
		if (obj instanceof Set)
			return ((Set<String>) obj).iterator();
		
		return new SingleIterator<String>( (String) obj );
	}
	
	/**
	 * Gets the names of the query terms.
	 * @return an iterator over the string names.
	 */
	public Iterator<String> getTermNames()
	{
		if (terms == null)
			return new EmptyIterator<String>();
		
		return terms.keySet().iterator();
	}
	
	/**
	 * Adds the specified query term to the set of query terms. Duplicate
	 * name/value pairs are suppressed.
	 * @param name
	 * @param value
	 */
	@SuppressWarnings(value={"unchecked"})
	public void addTerm( String name, String value )
	{
		ensureTerms();
		
		Object obj = terms.get( name );
		if (obj == null)
		{
			terms.put( name, value );
			return;
		}
		
		if (obj instanceof Set)
		{
			((Set<String>) obj).add( value );
			return;
		}
		
		Set<String> values = new HashSet<String>();
		values.add( (String) obj );
		values.add( value );
		terms.put( name, values );
	}

	/**
	 * Adds the specified query term to the set of query terms. Duplicate
	 * name/value pairs are suppressed.
	 * @param name
	 * @param value
	 */
	public void addTerm( String name, Integer value )
	{
		addTerm( name, value.toString() );
	}

	/**
	 * Adds the specified query term to the set of query terms. Duplicate
	 * name/value pairs are suppressed.
	 * @param name
	 * @param value
	 */
	public void addTerm( String name, Double value )
	{
		addTerm( name, value.toString() );
	}

	/**
	 * Adds the specified query term to the set of query terms. Duplicate
	 * name/value pairs are suppressed.
	 * @param name
	 * @param value
	 */
	public void addTerm( String name, int value )
	{
		addTerm( name, valueToString( value ) );
	}

	/**
	 * Adds the specified query term to the set of query terms. Duplicate
	 * name/value pairs are suppressed.
	 * @param name
	 * @param value
	 */
	public void addTerm( String name, double value )
	{
		addTerm( name, valueToString( value ) );
	}
	
	/**
	 * Removes all the values associated with the specified query term.
	 * @param name
	 * @return true if something was removed, false otherwise.
	 */
	public boolean removeTerm( String name )
	{
		return terms.remove( name ) != null;
	}
	
	/**
	 * Removes the specified name/value pair from the set of query terms.
	 * @param name
	 * @param value
	 * @return true if the name/value pair was found and removed.
	 */
	@SuppressWarnings(value={"unchecked"})
	public boolean removeTerm( String name, String value )
	{
		if (terms == null)
			return false;
		
		Object obj = terms.get( name );
		if (obj == null)
			return false;
		
		if (obj instanceof Set)
		{
			Set<String> x = (Set<String>) obj;
			boolean ok = x.remove( value );
			if (x.size() == 1)
				terms.put( name, x.iterator().next() );
			return ok;
		}
		
		terms.remove( name );
		return true;
	}
	
	/**
	 * Removes all the query terms.
	 */
	public void clearTerms()
	{
		if (terms != null)
			terms.clear();
	}
	
	private void ensureTerms()
	{
		if (terms == null)
			terms = new HashMap<String,Object>();
	}

	//////////////
	// FRAGMENT //
	//////////////

	/**
	 * @return the fragment.
	 */
	public String getFragment()
	{
		return fragment;
	}

	/**
	 * Sets the fragment.
	 * 
	 * @param fragment the fragment to set which may be null but not blank.
	 */
	public void setFragment( String fragment )
	{
		checkNotBlank( "fragment", fragment );
		this.fragment = fragment;
	}

	//////////
	// UTIL //
	//////////

	@Override
	public String toString()
	{
		StringBuffer sb = new StringBuffer();
		
		escape( sb, scheme );
		sb.append( ':' );
		
		if (host != null)
		{
			sb.append( "//" );
			if (user != null)
			{
				escape( sb, user );
				if (password != null)
				{
					sb.append( ':' );
					escape( sb, password );
				}
				sb.append( '@' );
			}
			escape( sb, host );
			if (port != 0)
			{
				sb.append( ':' );
				sb.append( port );
			}
			sb.append( '/' );
		}
		
		if (uri != null)
			escape( sb, uri );
		
		if (hasParams())
			paramsToString( sb );
		
		if (hasTerms())
			termsToString( sb );
		
		if (fragment != null)
		{
			sb.append( '#' );
			escape( sb, fragment );
		}
		
		return sb.toString();
	}

	@Override
	public boolean equals( Object obj )
	{
		if (obj == this)
			return true;
		
		if (!(obj instanceof URL))
			return false;
		
		URL other = (URL) obj;
		
		if (!StringUtil.equalsIgnoreCase( scheme, other.scheme ))
			return false;
		
		if (!StringUtil.equals( user, other.user ))
			return false;
		
		if (!StringUtil.equals( password, other.password ))
			return false;
		
		if (!StringUtil.equals( host, other.host ))
			return false;
		
		if (port != other.port)
			return false;
		
		if (!compareParams( params, other.params ))
			return false;
		
		if (!compareTerms( terms, other.terms ))
			return false;
		
		if (!StringUtil.equals( fragment, other.fragment ))
			return false;
		
		return true;
	}
	
	private boolean compareParams( Set a, Set b )
	{
		if (a == b)
			return true;
		
		int na = a != null ? a.size() : 0;
		int nb = b != null ? b.size() : 0;
		
		if (na == 0 && nb == 0)
			return true; // both empty
		
		if (na == 0 || nb == 0)
			return false; // one or the other empty
		
		return a.equals( b );
	}

	private boolean compareTerms( Map a, Map b )
	{
		if (a == b)
			return true;
		
		int na = a != null ? a.size() : 0;
		int nb = b != null ? b.size() : 0;
		
		if (na == 0 && nb == 0)
			return true; // both empty
		
		if (na == 0 || nb == 0)
			return false; // one or the other empty
		
		return a.equals( b );
	}

	private void paramsToString( StringBuffer sb )
	{
		Iterator<String> i = getParams();
		while (i.hasNext())
		{
			String param = i.next();
			sb.append( ';' );
			escape( sb, param );
		}
	}

	private void termsToString( StringBuffer sb )
	{
		char sep = '?';
		Iterator<String> i = getTermNames();
		while (i.hasNext())
		{
			String name = i.next();
			Iterator<String> j = getTerms( name );
			while (j.hasNext())
			{
				String value = j.next();
				sb.append( sep );
				escape( sb, name );
				sb.append( '=' );
				escape( sb, value );
				sep = '&';
			}
		}
	}

	/**
	 * Dumps the url contents for easy viewing.
	 */
	public void dump()
	{
		System.out.println( "---------------" );
		System.out.println( "scheme = "+scheme );
		System.out.println( "user = "+user );
		System.out.println( "password = "+password );
		System.out.println( "host = "+host );
		System.out.println( "port = "+port );
		System.out.println( "uri = "+uri );
		System.out.println( "params = "+params );
		System.out.println( "terms = "+terms );
		System.out.println( "fragment = "+fragment );
	}

	private static void escape( StringBuffer sb, String s )
	{
		CharIterator i = new CharIterator( s );
		while (i.hasNext())
		{
			char c = i.next();
			if (isEscaped( c ))
			{
				sb.append( '%' );
				sb.append( StringUtil.toHex( (c >>> 4) & 15 ) );
				sb.append( StringUtil.toHex( c & 15 ) );
			}
			else if (c == ' ')
			{
				sb.append( '+' );
			}
			else
			{
				sb.append( c );
			}
		}
	}

	private static boolean isEscaped( char c )
	{
		if (c >= '0' && c <= '9') return false;
		if (c >= 'A' && c <= 'Z') return false;
		if (c >= 'a' && c <= 'z') return false;
		if (c == ' ') return false;
		if (c == ',') return false;
		if (c == '.') return false;
		if (c == '/') return false;
		if (c == '!') return false;
		if (c == '$') return false;
		if (c == '^') return false;
		if (c == '*') return false;
		if (c == '(') return false;
		if (c == ')') return false;
		if (c == '_') return false;
		if (c == '-') return false;
		if (c == '{') return false;
		if (c == '}') return false;
		if (c == '[') return false;
		if (c == ']') return false;
		if (c == '|') return false;
		if (c == '\\') return false;
		return true;
	}

	private String unescape( String s )
	{
		StringBuffer sb = new StringBuffer();
		CharIterator i = new CharIterator( s );
		while (i.hasNext())
		{
			char c = i.next();
			if (c == '%')
			{
				char c1 = i.next();
				char c2 = i.next();
				sb.append( (char) ((StringUtil.fromHex( c1 ) << 4) | StringUtil.fromHex( c2 )) );
			}
			else if (c == '+')
			{
				sb.append( ' ' );
			}
			else
			{
				sb.append( c );
			}
		}
		return sb.toString();
	}

	private static void checkNotBlank( String name, String value )
	{
		if (value != null && value.length() == 0)
			throw new IllegalArgumentException( name+" is blank" );
	}
	
	private static String valueToString( int value )
	{
		return Integer.toString( value );
	}
	
	private static String valueToString( double value )
	{
		return Double.toString( value );
	}

	private static Set<String> copyParams( Set<String> params )
	{
		if (params == null)
			return null;
		
		return new HashSet<String>( params );
	}
	
	@SuppressWarnings(value={"unchecked"})
	private static Map<String,Object> copyTerms( Map<String,Object> terms )
	{
		if (terms == null)
			return null;
		
		Map<String,Object> map = new HashMap<String,Object>( terms );
		Iterator<Map.Entry<String,Object>> i = map.entrySet().iterator();
		while (i.hasNext())
		{
			Map.Entry<String,Object> me = i.next();
			Object obj = me.getValue();
			if (obj instanceof Set)
				me.setValue( new HashSet<String>( (Set<String>) obj ) );
		}
		return map;
	}
}
