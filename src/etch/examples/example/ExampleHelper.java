package etch.examples.example;

import metreos.util.URL;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.Pool;
import etch.bindings.java.transport.DefaultDeliveryService;
import etch.bindings.java.transport.Messagizer;
import etch.bindings.java.transport.Resources;
import etch.bindings.java.transport.ServerFactory;
import etch.bindings.java.transport.Transport;
import etch.bindings.java.transport.TransportFactory;
import etch.bindings.java.transport.TransportHelper;

/**
 * Transport helper for Example.
 */
public class ExampleHelper extends TransportHelper
{
	private ExampleHelper() { /* nothing */ }
	
	/**
	 * Constructs a new server session listener per specifications
	 * in uri and resources.
	 * 
	 * @param uri contains specifications for the server session
	 * listener and for the server session transport stack.
	 * 
	 * @param resources additional resources to aid in constructing
	 * new server sessions.
	 * 
	 * @param implFactory called upon accepting a new server session to
	 * construct a new server implementation to handle messages
	 * from the client during the session.
	 * 
	 * @return an out-of-band source that may be used to control
	 * the listener.
	 * 
	 * @throws Exception 
	 */
	public static Transport newExampleListener( URL uri,
		Resources resources, final ExampleServerFactory implFactory )
		throws Exception
	{
		final Resources res = initResources( resources );
		
		final TransportFactory factory = TransportFactory.get( uri.getScheme() );
		
		return factory.newListener( uri, res, new ServerFactory()
		{
			public void newServer( DefaultDeliveryService d ) throws Exception
			{
				ExampleClient client = new RemoteExampleClient( d );
				ExampleServer server = implFactory.newExampleServer( client );
				Pool qp = (Pool) res.get( QUEUED_POOL );
				Pool fp = (Pool) res.get( FREE_POOL );
				d.addStub( new StubExampleServer( d.getSource(), server, qp, fp ) );
			}

			public ValueFactory newValueFactory()
			{
				return new ValueFactoryExample();
			}
		} );
	}

	/**
	 * Constructs a new client session per specifications in uri
	 * and resources.
	 * 
	 * @param uri contains specifications for the client session
	 * transport stack.
	 * 
	 * @param resources additional resources to aid in constructing
	 * new sessions.
	 * 
	 * @param client an client implementation to handle any
	 * messages from the server during the session.
	 * 
	 * @return new remote server instance initialized by uri
	 * and resources which may be used to send messages to the
	 * server session.
	 * 
	 * @throws Exception 
	 */
	public static RemoteExampleServer newExampleServer( URL uri,
		Resources resources, ExampleClient client )
		throws Exception
	{
		resources = initResources( resources );
		resources = new Resources( resources );
		resources.put( Messagizer.VALUE_FACTORY, new ValueFactoryExample() );
			
		TransportFactory factory = TransportFactory.get( uri.getScheme() );
		
		DefaultDeliveryService d = factory.newTransport( uri, resources );
		
		if (client != null)
			d.addStub( new StubExampleClient( null,
				client,
				(Pool) resources.get( QUEUED_POOL ),
				(Pool) resources.get( FREE_POOL ) ) );
		
		return new RemoteExampleServer( d );
	}

	/**
	 * Factory for a Listener to use to create new instances of ExampleServer.
	 */
	public interface ExampleServerFactory
	{
		/**
		 * @param client an ExampleClient instance for server implementation
		 * to use to call back to the client.
		 * @return an instance of ExampleServer (e.g., ImplExampleServer) to
		 * handle messages which arrive from the client.
		 * @throws Exception 
		 */
		public ExampleServer newExampleServer( ExampleClient client )
			throws Exception;
	}

}
