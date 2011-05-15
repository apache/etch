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

package org.apache.etch.bindings.java.support;

import java.io.IOException;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.StringTokenizer;

import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.bindings.java.transport.TcpTransportFactory;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.bindings.java.transport.UdpTransportFactory;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.io.Transport;


/**
 * Interface to transport factory.
 */
abstract public class TransportFactory
{
	/**
	 * Constructs a new Transport stack topped by a TransportMessage
	 * which is used by the remote service implementations to send
	 * messages.
	 * @param uri transport configuration parameters.
	 * @param resources additional resources needed by the stack.
	 * @return the TransportMessage topping the transport stack.
	 * @throws Exception
	 */
	abstract protected TransportMessage newTransport( String uri,
		Resources resources ) throws Exception;

	/**
	 * Constructs a new Transport stack topped by a TransportMessage
	 * which is used by the remote service implementations to send
	 * messages.
	 * @param uri transport configuration parameters.
	 * @param resources additional resources needed by the stack.
	 * @return the TransportMessage topping the transport stack.
	 * @throws Exception
	 */
	static public TransportMessage getTransport( String uri,
		Resources resources ) throws Exception
	{
		URL u = new URL( uri );
		TransportFactory f = getTransportFactory( u.getScheme() );
		return f.newTransport( uri, resources );
	}

	/**
	 * Constructs a new Transport Listener which is used to construct
	 * server sessions.
	 * @param uri listener configuration parameters.
	 * @param resources additional resources needed by the listener.
	 * @return an out-of-band source which may be used to control the listener.
	 * @throws Exception
	 */
	abstract protected Transport<ServerFactory> newListener( String uri,
		Resources resources ) throws Exception;

	/**
	 * Constructs a new Transport Listener which is used to construct
	 * server sessions.
	 * @param uri listener configuration parameters.
	 * @param resources additional resources needed by the listener.
	 * @return an out-of-band source which may be used to control the listener.
	 * @throws Exception
	 */
	static public Transport<ServerFactory> getListener( String uri,
		Resources resources ) throws Exception
	{
		URL u = new URL( uri );
		TransportFactory f = getTransportFactory( u.getScheme() );
		return f.newListener( uri, resources );
	}

	/**
	 * Adds any message filters specified on the uri. They are added in order
	 * from transport to session. The first filter is the session for Messagizer,
	 * the second is the session for the first, etc. The last filter added is
	 * returned, and becomes the TransportMessage for what follows.
	 * @param transport
	 * @param uri
	 * @param resources
	 * @return the newly added filter
	 */
	protected TransportMessage addFilters( TransportMessage transport, URL uri,
		Resources resources )
	{
		String s = uri.getTerm( FILTER );
		if (s == null || s.length() == 0)
			return transport;
		
		StringTokenizer st = new StringTokenizer( s, ",:;| \t\r\n" );
		while (st.hasMoreTokens())
		{
			String t = st.nextToken();
			transport = addFilter( transport, t, uri, resources );
		}
		
		return transport;
	}
	
	/**
	 * Query term on the transport uri which defines a set of filters which
	 * process messages as they move up and down the transport stack. Filter
	 * names are separated by one or more of these characters: ",:;| \t\r\n".
	 */
	public final static String FILTER = "filter";

	private TransportMessage addFilter( TransportMessage transport, String name,
		URL uri, Resources resources )
	{
		String className = getFilter( name );
		
		if (className == null)
			throw new IllegalArgumentException(
				String.format( "Filter name '%s' not defined", name ) );
		
		Class<?> clss;
		
		try
		{
			clss = Class.forName( className );
		}
		catch ( ClassNotFoundException e )
		{
			throw new IllegalArgumentException(
				String.format( "Class '%s' for filter name '%s' not defined",
					className, name ) );
		}
		
		if (!TransportMessage.class.isAssignableFrom( clss ))
			throw new IllegalArgumentException(
				String.format( "Class '%s' for filter name '%s' does not implement TransportMessage",
					className, name ) );
		
		if (!SessionMessage.class.isAssignableFrom( clss ))
			throw new IllegalArgumentException(
				String.format( "Class '%s' for filter name '%s' does not implement SessionMessage",
					className, name ) );
		
		Constructor<?> c;
		
		try
		{
			c = clss.getConstructor( PARAMS );
		}
		catch ( SecurityException e )
		{
			throw new IllegalArgumentException(
				String.format( "Constructor %s(TransportMessage, URL, Resources) for filter name '%s' cannot be accessed",
					className, name ) );
		}
		catch ( NoSuchMethodException e )
		{
			throw new IllegalArgumentException(
				String.format( "Constructor %s(TransportMessage, URL, Resources) for filter name '%s' not defined",
					className, name ) );
		}
		
		try
		{
			return (TransportMessage) c.newInstance( new Object[] { transport, uri, resources } );
		}
		catch ( IllegalArgumentException e )
		{
			throw new IllegalArgumentException(
				String.format( "Constructor %s(TransportMessage, URL, Resources) for filter name '%s' failed",
					className, name ), e );
		}
		catch ( InstantiationException e )
		{
			throw new IllegalArgumentException(
				String.format( "Constructor %s(TransportMessage, URL, Resources) for filter name '%s' failed",
					className, name ), e );
		}
		catch ( IllegalAccessException e )
		{
			throw new IllegalArgumentException(
				String.format( "Constructor %s(TransportMessage, URL, Resources) for filter name '%s' failed",
					className, name ), e );
		}
		catch ( InvocationTargetException e )
		{
			throw new IllegalArgumentException(
				String.format( "Constructor %s(TransportMessage, URL, Resources) for filter name '%s' failed",
					className, name ), e.getTargetException() );
		}
	}
	
	/**
	 * @param name the filter name.
	 * @return the fully qualified class name of the filter.
	 */
	public static String getFilter( String name )
	{
		return filters.get( name );
	}
	
	/**
	 * Sets the fully qualified class name of the filter.
	 * @param name the filter name.
	 * @param className the fully qualified class name of the filter.
	 */
	public static void defineFilter( String name, String className )
	{
		filters.put( name, className );
	}
	
	private final static Map<String, String> filters =
		Collections.synchronizedMap( new HashMap<String, String>() );
	
	private final static Class<?>[] PARAMS = { TransportMessage.class, URL.class, Resources.class };
	
	static
	{
		defineFilter( "KeepAlive", "org.apache.etch.bindings.java.transport.filters.KeepAlive" );
		defineFilter( "PwAuth", "org.apache.etch.bindings.java.transport.filters.PwAuth" );
		defineFilter( "Logger", "org.apache.etch.bindings.java.transport.filters.Logger" );
	}
	
	/**
	 * Gets the named transport factory.
	 * @param name the name of a configured transport factory.
	 * @return the named transport factory.
	 * @throws Exception if the transport factory is not defined.
	 */
	private static TransportFactory getTransportFactory( String name ) throws Exception
	{
		Object tfspec = get( name );
		
		if (tfspec == null)
			throw new IOException( "transport factory scheme '"+name+"' unknown" );
		
		if (tfspec instanceof String)
			tfspec = Class.forName( (String) tfspec );
		
		if (tfspec instanceof Class<?>)
			tfspec = ((Class<?>) tfspec).newInstance();
		
		if (tfspec instanceof TransportFactory)
			return (TransportFactory) tfspec;
		
		throw new IllegalArgumentException(String.format(
			"Cannot create an instance of TransportFactory from transport factory name '%s'", name ) );
	}
	
	/**
	 * Gets the named transport factory spec.
	 * @param name
	 * @return the transport factory spec.
	 */
	public static Object get(String name)
	{
		return transportFactories.get( name );
	}
	
	/**
	 * Defines a transport factory.
	 * @param name the uri scheme of this transport factory.
	 * @param tfspec the transport factory to associate with the name. This may
	 * either be an instance of TransportFactory or a class which is a subclass
	 * of TransportFactory or a string which is a name of a class which is a
	 * subclass of transport factory.
	 */
	public static void define( String name, Object tfspec )
	{
		transportFactories.put( name, tfspec );
	}
	
	private final static Map<String, Object> transportFactories =
		Collections.synchronizedMap( new HashMap<String, Object>() );
	
	static
	{
		define( "tcp", new TcpTransportFactory( false ) );
		define( "tls", new TcpTransportFactory( true ) );
		define( "udp", new UdpTransportFactory( ) );
	}
}