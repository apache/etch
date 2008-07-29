package etch.examples.benchmark.etch;

import metreos.core.io.ListenerHandler;
import metreos.core.io.TcpConnection;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.Pool;
import etch.bindings.java.transport.DefaultDeliveryService2;
import etch.bindings.java.transport.DefaultListenerHandler;
import etch.bindings.java.transport.DefaultMessageHandler;
import etch.bindings.java.transport.MailboxMessageSource;
import etch.bindings.java.transport.MessageHandler;

/**
 * Helper for constructing Perf transports.
 */
public class PerfHelper
{
	/**
	 * @param queuedPool
	 * @param freePool
	 * @return a ListenerHandler to use.
	 */
	public static ListenerHandler newPerfListener( final Pool queuedPool,
		final Pool freePool )
	{
		return new DefaultListenerHandler()
		{
			@Override
			protected MessageHandler<MailboxMessageSource> newMessageHandler()
			{
				return new DefaultMessageHandler<StubPerfServer>()
				{
					@Override
					protected StubPerfServer newStub( MailboxMessageSource src )
					{
						return new StubPerfServer( src, new ImplPerfServer(),
							queuedPool, freePool );
					}
	
					@Override
					public String toString()
					{
						return "PerfServerMessageHandler";
					}
				};
			}
	
			@Override
			protected ValueFactory newValueFactory()
			{
				return new ValueFactoryPerf();
			}
	
			@Override
			public String toString()
			{
				return "PerfServerListener";
			}
		};
	}

	/**
	 * @param c
	 * @param client
	 * @param queuedPool
	 * @param freePool
	 * @return a PerfServer to use.
	 */
	public static PerfServer newPerfServer( TcpConnection c,
		final PerfClient client, final Pool queuedPool, final Pool freePool )
	{
		DefaultDeliveryService2<StubPerfClient> dds =
			new DefaultDeliveryService2<StubPerfClient>()
		{
			@Override
			protected StubPerfClient newStub( MailboxMessageSource src )
			{
				return new StubPerfClient( src, client, queuedPool, freePool );
			}

			@Override
			public String toString()
			{
				// TODO Auto-generated method stub
				return null;
			}
		};
		
		DefaultDeliveryService2<StubPerfClient> svc = DefaultDeliveryService2.setUp( dds,
			c, new ValueFactoryPerf() );
		
		return new RemotePerfServer( svc );
	}
}
