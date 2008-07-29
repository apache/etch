package etch.examples.authentication;

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
import etch.examples.locator.LocatorHelper;
import etch.examples.locator.Locator.Registration;

/**
 * @author bchampak
 */
public class AuthenticatorHelper
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
	 * @return AuthenticatorServer instance
	 * @throws Exception
	 */
	public static AuthenticatorServer getAuthenticatorInstance( String host,
	    int port, String instance ) throws Exception
	{
		if (instance.equals( LOCAL ))
			return getAuthenticator( host, port );

		Registration[] c = LocatorHelper.query( host, port,
			new URL( String.format( "etch://%s/tcp/%s", "Authenticator", instance ) ) );
		
		if (c.length == 0)
			throw new Exception( String.format( "could not find instance of %s/%s\n",
			    "Authenticator", instance ) );
		
		return getAuthenticator( c[0].contact.getHost(), c[0].contact.getPort() );
	}

	/**
	 * @param host
	 * @param port
	 * @return AuthenticatorServer instance
	 * @throws Exception
	 */
	public static AuthenticatorServer getAuthenticator( String host, int port )
	    throws Exception
	{
		ValueFactoryAuthenticator vf = new ValueFactoryAuthenticator();

		DefaultDeliveryService2<StubAuthenticator> dds = new DefaultDeliveryService2<StubAuthenticator>()
		{
			@Override
			protected StubAuthenticator newStub( MailboxMessageSource src )
			{
				return null;
			}

			@Override
			public String toString()
			{
				return "AuthenticatorServerDeliveryService";
			}
		};
		
		System.out.printf( "Connecting to AuthenticatorServer at %s:%d...\n", host, port );

		TcpConnection c = new TcpConnection( null, host, port, 0 );

		DeliveryService svc = DefaultDeliveryService2.setUp( dds, c, vf );

		final AuthenticatorServer as = new RemoteAuthenticatorServer( svc );

		c.start();
		c.waitUp( 4000 );
		
		System.out.printf( "Connected to AuthenticatorServer at %s:%d.\n", host, port );

		return as;
	}

	/**
	 * @param auth
	 * @return a constructed ListenerHandler.
	 */
	public static ListenerHandler newListenerHandler(
	    final AuthDb auth )
	{
		return new DefaultListenerHandler()
		{
			@Override
			protected MessageHandler<MailboxMessageSource> newMessageHandler()
			{
				return new DefaultMessageHandler<StubAuthenticatorServer>()
				{
					@Override
					protected StubAuthenticatorServer newStub(
					    MailboxMessageSource src )
					{
						return new StubAuthenticatorServer( src,
						    new AuthenticatorServerImpl( auth, false ), null,
						    null );
					}

					@Override
					public String toString()
					{
						return "AuthenticatorServerMessageHandler";
					}
				};
			}

			@Override
			protected ValueFactory newValueFactory()
			{
				return new ValueFactoryAuthenticator();
			}

			@Override
			public String toString()
			{
				return "AuthenticatorServerListener";
			}
		};
	}
}
