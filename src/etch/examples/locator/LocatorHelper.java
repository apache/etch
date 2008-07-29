package etch.examples.locator;

import java.util.List;

import metreos.core.io.ListenerHandler;
import metreos.core.io.TcpConnection;
import metreos.util.URL;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.DeliveryService;
import etch.bindings.java.transport.DefaultDeliveryService2;
import etch.bindings.java.transport.DefaultListenerHandler;
import etch.bindings.java.transport.DefaultMessageHandler;
import etch.bindings.java.transport.MailboxMessageSource;
import etch.bindings.java.transport.MessageHandler;
import etch.examples.locator.Locator.Registration;

/**
 * @author bchampak
 */
public class LocatorHelper
{
	/**
	 * @param host
	 * @param port
	 * @return LocatorServer instance
	 * @throws Exception
	 */
	public static LocatorServer getLocator( String host, int port )
	    throws Exception
	{
		ValueFactoryLocator vf = new ValueFactoryLocator();

		DefaultDeliveryService2<StubLocator> dds = new DefaultDeliveryService2<StubLocator>()
		{
			@Override
			protected StubLocator newStub( MailboxMessageSource src )
			{
				return null;
			}

			@Override
			public String toString()
			{
				return "TestRemoteLocatorServer";
			}
		};

		TcpConnection c = new TcpConnection( null, host, port, 0 );

		System.out.printf( "Connecting to LocatorServer at %s:%d...\n", host, port );

		DeliveryService svc = DefaultDeliveryService2.setUp( dds, c, vf );

		final LocatorServer ls = new RemoteLocatorServer( svc );

		c.start();
		c.waitUp( 4000 );

		System.out.printf( "Connected to LocatorServer at %s:%d.\n", host, port );

		return ls;
	}

	/**
	 * @param contexts
	 * @return the constructed ListenerHandler
	 */
	public static ListenerHandler newListenerHandler(
	    final List<Registration> contexts )
	{
		return new DefaultListenerHandler()
		{
			@Override
			protected MessageHandler<MailboxMessageSource> newMessageHandler()
			{
				return new DefaultMessageHandler<StubLocatorServer>()
				{
					@Override
					protected StubLocatorServer newStub(
					    MailboxMessageSource src )
					{
						return new StubLocatorServer( src, new LocatorServerImpl(
						    contexts ), null, null );
					}

					@Override
					public String toString()
					{
						return "LocatorServerMessageHandler";
					}
				};
			}

			@Override
			protected ValueFactory newValueFactory()
			{
				return new ValueFactoryLocator();
			}

			@Override
			public String toString()
			{
				return "LocatorServerListener";
			}
		};
	}

	/**
	 * @param host
	 * @param port
	 * @param type
	 * @param instance
	 * @return the context found
	 * @throws Exception
	 */
	public static Registration[] query( String host, int port, URL query ) throws Exception
	{
		LocatorServer ls = getLocator( host, port );
		Registration[] c = ls.find( query, 0, Integer.MAX_VALUE );
		((RemoteLocatorServer) ls)._stop();
		return c;
	}
}
