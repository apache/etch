package org.apache.etch.services.ns.support;

import java.util.Map;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DefaultValueFactory;
import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.bindings.java.support.TransportFactory;
import org.apache.etch.bindings.java.support.TransportHelper;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.services.ns.NameService.Entry;
import org.apache.etch.util.Monitor;
import org.apache.etch.util.Resources;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.Transport;
import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Ignore;
import org.junit.Test;

/** Test of EtchTransportFactory */
public class TestEtchTransportFactory
{
	private MockNSLib nsLibInstance;
	
	/** @throws Exception */
	@BeforeClass
	public static void setup() throws Exception
	{
		Class.forName( "org.apache.etch.services.ns.support.EtchTransportFactory" );
	}

	/** @throws Exception */
	@Before
	public void setupNsLibInstance() throws Exception
	{
		nsLibInstance = new MockNSLib();
		NSLib.setInstance( nsLibInstance );
	}
	
	/** @throws Exception */
	@After
	public void finiNsLibInstance() throws Exception
	{
		NSLib.setInstance( null );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void testListenerUri1() throws Exception
	{
		String uri = "etch:sourceListener?suri=&curi=tcp://10.89.30.202:4001&nsuri=namingServiceUri";
		TransportFactory.getListener( uri, null );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void testListenerUri2() throws Exception
	{
		String uri = "etch:sourceListener?curi=tcp://10.89.30.202:4001&nsuri=namingServiceUri";
		TransportFactory.getListener( uri, null );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void testListenerUri3() throws Exception
	{
		String uri = "etch:sourceListener?suri=tcp://0.0.0.0:4001&nsuri=namingServiceUri";
		TransportFactory.getListener( uri, null );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void testListenerUri4() throws Exception
	{
		String uri = "etch:sourceListener?suri=tcp://0.0.0.0:4001&curi=&nsuri=namingServiceUri";
		TransportFactory.getListener( uri, null );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void testListenerUri5() throws Exception
	{
		String uri = "etch:suri=tcp://0.0.0.0:4001&curi=tcp://10.89.30.202:4001&nsuri=namingServiceUri";
		TransportFactory.getListener( uri, null );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void testClientUri1() throws Exception 
	{
		String uri = "etch://127.0.0.1:8002/";
		TransportFactory.getTransport( uri, null );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void testClientUri2() throws Exception 
	{
		String uri = "etch:";
		TransportFactory.getTransport( uri, null );
	}

	/** @throws Exception */
	@Test
	public void testListener1() throws Exception
	{
		String uri = "etch:sourceListener?suri=tcp://0.0.0.0:4001&curi=tcp://10.89.30.202:4001&nsuri=namingServiceUri";
		Transport<ServerFactory> tsf = TransportFactory.getListener( uri, null );
		tsf.setSession( new MySession() );
		
		// register should not be called before start is called on the transport
		nsLibInstance.method.waitUntilEq( "getDefaultNsUri" );
	}

	/** @throws Exception */
	@Test
	public void testListener2() throws Exception
	{
		String uri = "etch:sourceListener?suri=tcp://0.0.0.0:4001&curi=tcp://10.89.30.202:4001&nsuri=namingServiceUri";
		Transport<ServerFactory> tsf = TransportFactory.getListener( uri, null );
		tsf.setSession( new MySession() );
		tsf.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		nsLibInstance.method.waitUntilEq( "register" );
	}

	/** @throws Exception */
	@Test
	public void testClient1() throws Exception
	{
		String uri = "etch:sourceListener";
		TransportFactory.getTransport( uri, null );
		
		// no lookup should've been done so far
		nsLibInstance.method.waitUntilEq( "getDefaultNsUri" );
	}

	/** @throws Exception */
	@Test @Ignore
	public void testCommunication1() throws Exception
	{
		String uri = "etch:sourceListener?suri=tcp://0.0.0.0:8001&curi=tcp://10.89.30.202:4001&nsuri=namingServiceUri";
		
		Resources res = TransportHelper.initResources( null );
		res.put( Transport.VALUE_FACTORY, new DefaultValueFactory( uri, new TypeMap(), new Class2TypeMap() ) );
		Transport<ServerFactory> tsf = TransportFactory.getListener( uri, res );
		tsf.setSession( new MySession() );
		tsf.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		nsLibInstance.method.waitUntilEq( "register" );
		
		String clientUri = "etch:sourceListener";
		
		Resources r = TransportHelper.initResources( null );
		r.put( Transport.VALUE_FACTORY, new DefaultValueFactory( clientUri, new TypeMap(), new Class2TypeMap() ) );
		
		TransportMessage tm = TransportFactory.getTransport( clientUri, r );
		tm.setSession( new MockSessionMessage() );
		tm.transportControl( Transport.START_AND_WAIT_UP, 4000 );
		
		nsLibInstance.method.waitUntilEq( "lookup" );
		
		Thread.sleep( 1000 );
		
		tsf.transportControl( Transport.STOP_AND_WAIT_DOWN, 4000 );
		
		System.in.read();
	}
	
	/**
	 * Mock NSLib implementation
	 */
	public class MockNSLib extends NSLib
	{
		private Monitor<String> method = new Monitor<String>( "method", "getDefaultNsUri" );

		@Override
		public String getDefaultNsUri()
		{
			method.set( "getDefaultNsUri" );
			return null;
		}

		@Override
		public Entry lookup( String uri )
		{
			method.set( "lookup" );
			return new Entry( "sourceListener", null, "tcp://127.0.0.1:8001", 0, null, null, false);
		}

		@Override
		public void register( Session session, String uri, Map<?, ?> qualities,
				int ttl )
		{
			method.set( "register" );
		}

		@Override
		public void setDefaultNsUri( String defaultNsUri )
		{
			method.set( "setDefaultNsUri" );		
		}

		@Override
		public void shutdown()
		{
			method.set( "shutdown" );		
		}

		@Override
		public void unregister( Session session, String uri, boolean deleteEntry )
		{
			method.set( "unregister" );
		}
	}
	
	/**
	 * Mock session 
	 *
	 */
	private class MySession implements ServerFactory
	{
		public void sessionControl( Object arg0, Object arg1 ) throws Exception
		{
			System.out.println( "session control" );
		}

		public void sessionNotify( Object arg0 ) throws Exception
		{	
			System.out.println( "session notify: " + arg0 );
		}

		public Object sessionQuery( Object arg0 ) throws Exception
		{
			System.out.println( "sessionQuery: " + arg0 );
			return null;
		}

		public void newServer( String uri, Resources resources,
				TransportMessage transport ) throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public ValueFactory newValueFactory()
		{
			// TODO Auto-generated method stub
			return null;
		}

		public Session getSession()
		{
			// TODO Auto-generated method stub
			return null;
		}

		public void setSession( Session session )
		{
			// TODO Auto-generated method stub
			
		}

		public void transportControl( Object control, Object value )
				throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public void transportNotify( Object event ) throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public Object transportQuery( Object query ) throws Exception
		{
			// TODO Auto-generated method stub
			return null;
		}
	}
	
	/** SessionMessage for testing */
	public class MockSessionMessage implements SessionMessage
	{
		public boolean sessionMessage( Who sender, Message msg )
				throws Exception
		{
			// TODO Auto-generated method stub
			return false;
		}

		public void sessionControl( Object control, Object value )
				throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public void sessionNotify( Object event ) throws Exception
		{
			// TODO Auto-generated method stub
			
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			// TODO Auto-generated method stub
			return null;
		}
	}
	
}
