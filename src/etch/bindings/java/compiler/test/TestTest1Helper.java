package etch.bindings.java.compiler.test;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import etch.bindings.java.transport.TcpTransportFactory;
import etch.bindings.java.transport.TransportFactory;
import etch.util.core.io.Transport;

/** Test of Test1Helper */
public class TestTest1Helper
{
	private static Transport<?> listener;
	
	/** @throws Exception */
	@BeforeClass
	public static void init() throws Exception
	{
		TransportFactory.put( "rohit", new TcpTransportFactory() );
		
		String uri = "rohit://0.0.0.0:4040";
		
		listener = Test1Helper.newListener( uri, null, new MainTest1Listener() );
		
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}
	
	/** @throws Exception */
	@Test
	public void test() throws Exception
	{
		String uri = "rohit://localhost:4040";

		RemoteTest1Server server = Test1Helper.newServer( uri, null, new MainTest1Client() );
		server._startAndWaitUp( 4000 );

		// TODO nothing to do but wait for server to really be up.
		Thread.sleep( 1000 );
	
		server._stopAndWaitDown( 4000 );
		server = null;
	}
	
	/** @throws Exception */
	@AfterClass
	public static void fini() throws Exception
	{
		listener.transportControl( Transport.STOP_AND_WAIT_DOWN, 0 );
		listener = null;
	}
}
