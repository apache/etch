package etch.examples.log;

import metreos.core.io.ListenerHandler;
import metreos.core.io.TcpConnection;
import metreos.util.URL;
import etch.bindings.java.msg.ValueFactory;
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
public class LogHelper {
	
	/**
	 * Instance name for local log.
	 */
	public static final String LOCAL = ".local";

	/**
	 * @param host 
	 * @param port 
	 * @return LogServer instance
	 * @throws Exception
	 */
	public static LogServer getLog( String host, int port) throws Exception{
		
		ValueFactoryLog vf = new ValueFactoryLog();
		
		DefaultDeliveryService2<StubLog> dds = new DefaultDeliveryService2<StubLog>()
		{
			@Override
			protected StubLog newStub( MailboxMessageSource src )
			{
				return null;
			}
			
			@Override
			public String toString()
			{
				return "TestRemoteLogServer";
			}
		};
		
		TcpConnection c = new TcpConnection( null, host, port, 0 );
		
		DefaultDeliveryService2<StubLog> svc = 
			DefaultDeliveryService2.setUp(dds, c, vf );
		
		@SuppressWarnings("unused")
		final LogServer logs = new RemoteLogServer( svc );
		
		c.start();
		c.waitUp( 4000 );
		
		return logs;
	}

	/**
	 * @param auth
	 * @param lsink
	 * @return a ListenerHandler to use.
	 */
	public static ListenerHandler newListenerHandler( final AuthenticatorServer auth, final LogSink lsink )
	{
		return new DefaultListenerHandler()
		{
			@Override
			protected MessageHandler<MailboxMessageSource> newMessageHandler()
			{
				return new DefaultMessageHandler<StubLogServer>()
				{
					@Override
					protected StubLogServer newStub( MailboxMessageSource src )
					{
						return new StubLogServer( src,
							new LogServerImpl( auth, true, lsink ),
							null, null );
					}
	
					@Override
					public String toString()
					{
						return "LocatorServerListener";
					}
				};
			}
	
			@Override
			protected ValueFactory newValueFactory()
			{
				return new ValueFactoryLog();
			}
	
			@Override
			public String toString()
			{
				return "LogServerListener";
			}
		};
	}

	/**
	 * Creates a session to a log server.
	 * @param host
	 * @param port
	 * @param instance
	 * @return a LogServer instance to use.
	 * @throws Exception
	 */
	public static LogServer getLogInstance( String host, int port, String instance ) throws Exception
	{
		if (instance.equals( LOCAL ))
			return getLog( host, port );

		Registration[] c = LocatorHelper.query( host, port,
			new URL( String.format( "etch://%s/tcp/%s", "Log", instance ) ) );
		
		if (c.length == 0)
			throw new Exception( String.format( "could not find instance of %s/%s\n",
			    "Log", instance ) );
		
		return getLog( c[0].contact.getHost(), c[0].contact.getPort() );
	}
}
