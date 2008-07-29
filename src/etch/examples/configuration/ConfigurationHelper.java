package etch.examples.configuration;

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
import etch.examples.authentication.AuthenticatorServer;
import etch.examples.locator.LocatorHelper;
import etch.examples.locator.Locator.Registration;

/**
 * @author bchampak
 */
public class ConfigurationHelper
{
	/**
	 * An instance name which means host and port are for an authenticator
	 * service and not a name service.
	 */
	public final static String LOCAL = ".local";
	
	/**
	 * @param host
	 * @param port
	 * @param instance
	 * @return ConfigurationServer instance
	 * @throws Exception
	 */
	public static ConfigurationServer getConfigurationInstance( String host,
	    int port, String instance ) throws Exception
	{
		if (instance.equals( LOCAL ))
			return getConfiguration( host, port );

		Registration[] c = LocatorHelper.query( host, port,
			new URL( String.format( "etch://%s/tcp/%s", "Configuration", instance ) ) );
		
		if (c.length == 0)
			throw new Exception( String.format( "could not find instance of %s/%s\n",
			    "Configuration", instance ) );
		
		return getConfiguration( c[0].contact.getHost(), c[0].contact.getPort() );
	}

	/**
	 * @param host
	 * @param port
	 * @return ConfigurationServer instance
	 * @throws Exception
	 */
	public static ConfigurationServer getConfiguration( String host, int port )
	    throws Exception
	{
		ValueFactoryConfiguration vf = new ValueFactoryConfiguration();

		DefaultDeliveryService2<StubConfiguration> dds = new DefaultDeliveryService2<StubConfiguration>()
		{
			@Override
			protected StubConfiguration newStub( MailboxMessageSource src )
			{
				return null;
			}

			@Override
			public String toString()
			{
				return "ConfigurationServerDeliveryService";
			}
		};
		
		System.out.printf( "Connecting to ConfigurationServer at %s:%d...\n", host, port );

		TcpConnection c = new TcpConnection( null, host, port, 0 );

		DeliveryService svc = DefaultDeliveryService2.setUp( dds, c, vf );

		final ConfigurationServer cs = new RemoteConfigurationServer( svc );

		c.start();
		c.waitUp( 4000 );
		
		System.out.printf( "Connected to ConfigurationServer at %s:%d.\n", host, port );

		return cs;
	}

	/**
	 * @param auth
	 * @param config
	 * @return a constructed ListenerHandler.
	 */
	public static ListenerHandler newListenerHandler(
	    final AuthenticatorServer auth, final ConfigurationServer config )
	{
		return new DefaultListenerHandler()
		{
			@Override
			protected MessageHandler<MailboxMessageSource> newMessageHandler()
			{
				return new DefaultMessageHandler<StubConfigurationServer>()
				{
					@Override
					protected StubConfigurationServer newStub(
					    MailboxMessageSource src )
					{
						return new StubConfigurationServer( src,
						    new ConfigurationServerImpl( auth, config, false ),
						    null, null );
					}

					@Override
					public String toString()
					{
						return "ConfigurationServerMessageHandler";
					}
				};
			}

			@Override
			protected ValueFactory newValueFactory()
			{
				return new ValueFactoryConfiguration();
			}

			@Override
			public String toString()
			{
				return "ConfigurationServerListener";
			}
		};
	}
}
