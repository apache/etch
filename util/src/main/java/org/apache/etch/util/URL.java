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

package org.apache.etch.util;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

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
		if (urlStr != null && urlStr.length() > 0)
			parse( urlStr );
	}
	
	/**
	 * Constructs a url from another url.
	 * @param other
	 */
	public URL( URL other )
	{
		scheme = other.scheme;
		user = other.user;
		password = other.password;
		host = other.host;
		port = other.port;
		uri = other.uri;
		params = copyList( other.params );
		terms = copyTerms( other.terms );
		fragment = other.fragment;
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
			String p = unescape( x[1] );
			checkNotInteger( "port", p );
			setPort( Integer.parseInt( p ) );
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
	
	private Integer port;
	
	private String uri;
	
	private List<String> params;
	
	private Map<String, Object> terms;
	
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
		//checkNotBlank( "password", password );
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
	 * @return true if a port has been specified, false otherwise.
	 */
	public boolean hasPort()
	{
		return port != null;
	}
	
	/**
	 * @return the port. Null if no port has been specified.
	 */
	public Integer getPort()
	{
		return port;
	}

	/**
	 * Sets the port.
	 * 
	 * @param port the port to set which may be between 0 and 65535.
	 */
	public void setPort( Integer port )
	{
		if (port != null && (port < 0 || port > 65535))
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
	 * @return the first param which starts with the specified prefix.
	 */
	public String getParam( String prefix )
	{
		checkNotNull( prefix, "prefix == null" );
		
		if (params == null)
			return null;
		
		for (String p: params)
			if (p.startsWith( prefix ))
				return p;
		
		return null;
	}
	
	/**
	 * @return an iterator over all the params. The params are strings, generally
	 * of the form "transport=tcp". But they can be anything you like, really.
	 * The iterator might be empty.
	 */
	public String[] getParams()
	{
		if (params == null)
			return new String[] {};
		
		return params.toArray( new String[] {} );
	}
	
	/**
	 * Adds a param to the list of params for this url. Only the unique
	 * params are maintained. Duplicate param values are suppressed.
	 * 
	 * @param param a param (e.g., "transport=tcp" or "01831864574898475").
	 */
	public void addParam( String param )
	{
		checkNotNull( param, "param == null" );
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
		checkNotNull( prefix, "prefix == null" );
		
		if (params == null)
			return null;
		
		for (String p: getParams())
		{
			if (p.startsWith( prefix ))
			{
				params.remove( p );
				return p;
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
			params = new ArrayList<String>();
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
		return terms != null && !terms.isEmpty();
	}
	
	/**
	 * @param name
	 * 
	 * @return true if there is at least one query term with the specified
	 * name.
	 */
	public boolean hasTerm( String name )
	{
		checkName( name );
		
		if (terms == null)
			return false;
		
		return terms.containsKey( name );
	}
	
	/**
	 * @param name
	 * @param value
	 * @return true if there is a query term with the specified value.
	 */
	@SuppressWarnings("unchecked")
	public boolean hasTerm( String name, String value )
	{
		checkName( name );
		
		if (terms == null)
			return false;
		
		if (value == null)
			return hasTerm( name );
		
		Object obj = terms.get( name );
		if (obj == null)
			return false;
		
		if (obj instanceof List)
			return ((List<String>) obj).contains( value );
		
		return obj.equals( value );
	}

	/**
	 * @param name
	 * @param value
	 * @return true if there is a query term with the specified value.
	 */
	public boolean hasTerm( String name, Integer value )
	{
		return hasTerm( name, toString( value ) );
	}

	/**
	 * @param name
	 * @param value
	 * @return true if there is a query term with the specified value.
	 */
	public boolean hasTerm( String name, Double value )
	{
		return hasTerm( name, toString( value ) );
	}

	/**
	 * @param name
	 * @param value
	 * @return true if there is a query term with the specified value.
	 */
	public boolean hasTerm( String name, Boolean value )
	{
		return hasTerm( name, toString( value ) );
	}
	
	/**
	 * @param name
	 * @return true if the query term specified by name has multiple values.
	 */
	@SuppressWarnings("unchecked")
	public boolean hasMultipleValues( String name )
	{
		checkName( name );
		
		if (terms == null)
			return false;
		
		Object obj = terms.get( name );
		if (obj == null)
			return false;
		
		if (obj instanceof List)
			return ((List<String>) obj).size() > 1;
		
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
	@SuppressWarnings("unchecked")
	public String getTerm( String name )
	{
		checkName( name );
		
		if (terms == null)
			return null;
		
		Object obj = terms.get( name );
		if (obj == null)
			return null;
		
		if (obj instanceof List)
		{
			Iterator<String> i = ((List<String>) obj).iterator();
			
			if (!i.hasNext())
				return null;
			
			String v = i.next();
			
			if (i.hasNext())
				throw new UnsupportedOperationException( String.format(
					"term '%s' has multiple values", name ) );
			
			return v;
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
	 * Gets the Integer value of the specified query term.
	 * @param name
	 * @return the Integer value, or null if not found.
	 * @see #getTerm(String)
	 */
	public Integer getIntegerTerm( String name )
	{
		String s = getTerm( name );
		if (s == null)
			return null;
		return Integer.valueOf( s );
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
		Integer value = getIntegerTerm( name );
		if (value == null)
			return defaultValue;
		return value;
	}
	
	/**
	 * Gets the Double value of the specified query term.
	 * @param name
	 * @return the Double value, or null if not found.
	 * @see #getTerm(String)
	 */
	public Double getDoubleTerm( String name )
	{
		String s = getTerm( name );
		if (s == null)
			return null;
		return Double.valueOf( s );
	}

	/**
	 * Gets the double value of the specified query term.
	 * @param name
	 * @param defaultValue the value to return if the term is not found.
	 * @return the double value, or defaultValue if not found.
	 * @see #getTerm(String)
	 */
	public double getDoubleTerm( String name, double defaultValue )
	{
		Double value = getDoubleTerm( name );
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
		return Boolean.valueOf( s );
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
		Boolean value = getBooleanTerm( name );
		if (value == null)
			return defaultValue;
		return value;
	}
	
	/**
	 * Gets the values of the specified query term.
	 * @param name
	 * @return an iterator over the string values of the query term. May be empty.
	 */
	@SuppressWarnings("unchecked")
	public String[] getTerms( String name )
	{
		checkName( name );
		
		if (terms == null)
			return new String[] {};
		
		Object obj = terms.get( name );
		if (obj == null)
			return new String[] {};
		
		if (obj instanceof List)
			return ((List<String>) obj).toArray( new String[] {} );
		
		return new String[] { (String) obj };
	}
	
	/**
	 * Gets the names of the query terms.
	 * @return an iterator over the string names.
	 */
	public String[] getTermNames()
	{
		if (terms == null)
			return new String[] {};
		
		return terms.keySet().toArray( new String[] {} );
	}
	
	/**
	 * Adds the specified query term to the list of query terms. Duplicate
	 * name/value pairs are suppressed.
	 * @param name
	 * @param value
	 */
	@SuppressWarnings("unchecked")
	public void addTerm( String name, String value )
	{
		checkName( name );
		
		if (value == null)
			return;
		
		ensureTerms();
		
		Object obj = terms.get( name );
		if (obj == null)
		{
			terms.put( name, value );
			return;
		}
		
		if (obj instanceof List)
		{
			List<String> values = (List<String>) obj;
			if (!values.contains( value ))
				values.add( value );
			return;
		}
		
		// obj is not a list but we need one, so replace obj in terms with a
		// list, then add value to the list.
		
		List<String> values = new ArrayList<String>();
		terms.put( name, values );
		values.add( (String) obj );

		values.add( value );
	}

	/**
	 * Adds the specified query term to the list of query terms. Duplicate
	 * name/value pairs are suppressed.
	 * @param name
	 * @param value
	 */
	public void addTerm( String name, Integer value )
	{
		addTerm( name, toString( value ) );
	}

	/**
	 * Adds the specified query term to the list of query terms. Duplicate
	 * name/value pairs are suppressed.
	 * @param name
	 * @param value
	 */
	public void addTerm( String name, Double value )
	{
		addTerm( name, toString( value ) );
	}

	/**
	 * Adds the specified query term to the list of query terms. Duplicate
	 * name/value pairs are suppressed.
	 * @param name
	 * @param value
	 */
	public void addTerm( String name, Boolean value )
	{
		addTerm( name, toString( value ) );
	}
	
	/**
	 * Removes all the values associated with the specified query term.
	 * @param name
	 * @return true if something was removed, false otherwise.
	 */
	public boolean removeTerm( String name )
	{
		checkName( name );
		
		if (terms == null)
			return false;
		
		return terms.remove( name ) != null;
	}
	
	/**
	 * Removes the specified name/value pair from the list of query terms.
	 * @param name
	 * @param value
	 * @return true if the name/value pair was found and removed.
	 */
	@SuppressWarnings("unchecked")
	public boolean removeTerm( String name, String value )
	{
		checkName( name );
		
		if (terms == null)
			return false;
		
		if (value == null)
			return removeTerm( name );
		
		Object obj = terms.get( name );
		if (obj == null)
			return false;
		
		if (obj instanceof List)
		{
			List<String> values = (List<String>) obj;
			boolean ok = values.remove( value );
			if (values.isEmpty())
				terms.remove( name );
			return ok;
		}
		
		if (obj.equals( value ))
		{
			terms.remove( name );
			return true;
		}
		
		return false;
	}

	/**
	 * Removes the specified name/value pair from the list of query terms.
	 * @param name
	 * @param value
	 * @return true if the name/value pair was found and removed.
	 */
	public boolean removeTerm( String name, Integer value )
	{
		return removeTerm( name, toString( value ) );
	}

	/**
	 * Removes the specified name/value pair from the list of query terms.
	 * @param name
	 * @param value
	 * @return true if the name/value pair was found and removed.
	 */
	public boolean removeTerm( String name, Double value )
	{
		return removeTerm( name, toString( value ) );
	}

	/**
	 * Removes the specified name/value pair from the list of query terms.
	 * @param name
	 * @param value
	 * @return true if the name/value pair was found and removed.
	 */
	public boolean removeTerm( String name, Boolean value )
	{
		return removeTerm( name, toString( value ) );
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
			terms = new HashMap<String, Object>();
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
			if (port != null)
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
	public int hashCode()
	{
		int code = 23547853;
		code ^= hc( scheme );
		code ^= hc( user );
		code ^= hc( password );
		code ^= hc( host );
		code ^= hc( port );
		code ^= hc( uri );
		code ^= hc( params );
		code ^= hc( terms );
		code ^= hc( fragment );
		return code;
	}
	
	private int hc( Map<String, Object> m )
	{
		return m != null ? m.hashCode() : 793;
	}

	private int hc( List<String> s )
	{
		return s != null ? s.hashCode() : 161;
	}

	private int hc( Integer i )
	{
		return i != null ? i.hashCode() : 59;
	}

	private int hc( String s )
	{
		return s != null ? s.hashCode() : 91;
	}

	@Override
	public boolean equals( Object obj )
	{
		if (obj == this)
			return true;
		
		if (obj == null)
			return false;
		
		if (obj.getClass() != URL.class)
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
		
		if (!eq( port, other.port ))
			return false;
		
		if (!StringUtil.equals( uri, other.uri ))
			return false;
		
		if (!eq( params, other.params ))
			return false;
		
		if (!eq( terms, other.terms ))
			return false;
		
		if (!StringUtil.equals( fragment, other.fragment ))
			return false;
		
		return true;
	}
	
	private boolean eq( Object a, Object b )
	{
		return a == b || a != null && b != null && a.equals( b );
	}

	private void paramsToString( StringBuffer sb )
	{
		for (String p: getParams())
		{
			sb.append( ';' );
			escape( sb, p );
		}
	}

	private void termsToString( StringBuffer sb )
	{
		char sep = '?';
		for (String n: getTermNames())
		{
			for (String v: getTerms( n ))
			{
				sb.append( sep );
				escape( sb, n );
				sb.append( '=' );
				escape( sb, v );
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
		if (s == null)
		{
			sb.append( "null" );
			return;
		}
		
		CharIterator i = new CharIterator( s );
		while (i.hasNext())
		{
			char c = i.next();
			if (isEscaped( c ))
			{
				sb.append( '%' );
				sb.append( StringUtil.toHex( c >>> 4 & 15 ) );
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

	private static String unescape( String s )
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
				sb.append( (char) (StringUtil.fromHex( c1 ) << 4 | StringUtil.fromHex( c2 )) );
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
	
	private static void checkName( String name )
	{
		if (name == null || name.length() == 0)
			throw new IllegalArgumentException( "name null or empty" );
	}

	private static String toString( Object value )
	{
		return value != null ? value.toString() : null;
	}
	
	private static void checkNotNull( Object v, String msg )
	{
		if (v == null)
			throw new NullPointerException( msg );
	}

	private static void checkNotBlank( String name, String value )
	{
		if (value != null && value.length() == 0)
			throw new IllegalArgumentException( name+" is blank" );
	}

	private static void checkNotInteger( String name, String value )
	{
		checkNotBlank( name, value );
		try
		{
			Integer.parseInt( value );
		}
		catch ( NumberFormatException e )
		{
			throw new IllegalArgumentException( name+" is not integer" );
		}
	}

	private static List<String> copyList( List<String> params )
	{
		// just goes one level deep.
		
		if (params == null)
			return null;
		
		return new ArrayList<String>( params );
	}
	
	@SuppressWarnings("unchecked")
	private static Map<String, Object> copyTerms( Map<String, Object> terms )
	{
		if (terms == null)
			return null;
		
		Map<String, Object> nterms = new HashMap<String, Object>();
		for (Map.Entry<String, Object> me: terms.entrySet())
		{
			String name = me.getKey();
			Object value = me.getValue();
			if (value instanceof List)
				value = copyList( (List<String>) value );
			nterms.put( name, value );
		}
		return nterms;
	}
}
