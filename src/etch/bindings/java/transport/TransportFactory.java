package etch.bindings.java.transport;

import java.io.IOException;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import etch.bindings.java.support.DeliveryService;
import etch.util.Resources;
import etch.util.URL;
import etch.util.core.io.Transport;

/**
 * Interface to transport factory.
 */
abstract public class TransportFactory
{
	/**
	 * Constructs a new Transport stack topped by a DeliveryService
	 * which is used by the remote service implementations to send
	 * messages and make calls.
	 * @param uri transport configuration parameters.
	 * @param resources additional resources needed by the stack.
	 * @return the DeliveryService topping the transport stack.
	 * @throws Exception
	 */
	abstract public DeliveryService newTransport( String uri,
		Resources resources ) throws Exception;

	/**
	 * Constructs a new Transport stack topped by a DeliveryService
	 * which is used by the remote service implementations to send
	 * messages and make calls.
	 * @param uri transport configuration parameters.
	 * @param resources additional resources needed by the stack.
	 * @return the DeliveryService topping the transport stack.
	 * @throws Exception
	 */
	static public DeliveryService getTransport( String uri,
		Resources resources ) throws Exception
	{
		URL u = new URL( uri );
		TransportFactory f = get( u.getScheme() );
		return f.newTransport( uri, resources );
	}

	/**
	 * Constructs a new Transport Listener which is used to construct
	 * server sessions.
	 * @param uri listener configuration parameters.
	 * @param resources additional resources needed by the listener.
	 * @param factory constructs the actual service class instances.
	 * @return an out-of-band source which may be used to control the listener.
	 * @throws Exception
	 */
	abstract public Transport<?> newListener( String uri,
		Resources resources, ServerFactory factory ) throws Exception;

	/**
	 * Constructs a new Transport Listener which is used to construct
	 * server sessions.
	 * @param uri listener configuration parameters.
	 * @param resources additional resources needed by the listener.
	 * @param factory constructs the actual service class instances.
	 * @return an out-of-band source which may be used to control the listener.
	 * @throws Exception
	 */
	static public Transport<?> getListener( String uri, Resources resources,
		ServerFactory factory ) throws Exception
	{
		URL u = new URL( uri );
		TransportFactory f = get( u.getScheme() );
		return f.newListener( uri, resources, factory );
	}
	
	private final static Map<String, TransportFactory> transportFactories =
		Collections.synchronizedMap( new HashMap<String, TransportFactory>() );
	
	/**
	 * Gets the named transport factory.
	 * @param name the name of a configured transport factory.
	 * @return the named transport factory.
	 * @throws IOException if the transport factory is not defined.
	 */
	public static TransportFactory get( String name ) throws IOException
	{
		TransportFactory tf = transportFactories.get( name );
		if (tf == null)
			throw new IOException( "transport factory scheme '"+name+"' unknown" );
		return tf;
	}
	
	/**
	 * Puts a named transport factory.
	 * @param name the uri scheme of this transport factory.
	 * @param tf the transport factory to associate with the name.
	 */
	public static void put( String name, TransportFactory tf )
	{
		transportFactories.put( name, tf );
	}
	
	static
	{
		put( "tcp", new TcpTransportFactory(false) );
		put("tls", new TcpTransportFactory(true));
	}
}