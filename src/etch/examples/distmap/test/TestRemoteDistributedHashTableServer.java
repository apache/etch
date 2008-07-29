package etch.examples.distmap.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

import java.net.SocketException;

import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.core.io.TcpConnection;

import org.junit.After;
import org.junit.AfterClass;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import etch.examples.distmap.DistributedHashTableHelper;
import etch.examples.distmap.DistributedHashTableServer;
import etch.examples.distmap.RemoteDistributedHashTableServer;
import etch.examples.distmap.DistributedHashTable.Entry;

/**
 * Tests the RemoteDistributedHashTableServer interface. Start the
 * TestRemoteDistributedHashTableListener first.
 */
public class TestRemoteDistributedHashTableServer
{

	TcpConnection c = null;
	DistributedHashTableServer server = null;
	public static Listener l = null;
	public static ListenerHandler lh = null;

	final boolean myBool1 = true;
	final byte myByte1 = 127;
	final short myShort1 = 32767;
	final int myInt1 = 2147483647;
	final long myLong1 = 9223372036854775807l;
	final float myFloat1 = 3.4028235e38f;
	final double myDouble1 = 1.7976931348623157e308d;
	final String myString1 = "abc";


	// connect to service //

	@BeforeClass
	public static void instantiateListener()
		throws Exception
	{
		lh = DistributedHashTableHelper.newDistributedHashTableServerListener( null, null );
		l = new Listener( lh, 5, null, 4001, 0 );
		l.start();
		l.waitUp( 4000 );
	}

	@AfterClass
	public static void shutListener()
		throws Exception
	{
		l.stop();
		l.waitDown( 4000 );
	}

	@Before
	public void makeConnection()
		throws Exception
	{
		c = new TcpConnection( null, "localhost", 4001, 0 );

		server =
		DistributedHashTableHelper.newDistributedHashTableServer( c, null,
			null, null );

		c.start();
		c.waitUp( 4000 );
	}

	@After
	public void shutConnection()
		throws Exception
	{
		try
		{
			((RemoteDistributedHashTableServer) server)._stop();
			c.waitDown( 4000 );
		}
		catch ( SocketException e )
		{
			if ("socket closed".equals( e.getMessage() ))
				return;
			throw e;
		}
	}


	@Test
	public void testBoolean()
		throws Exception
	{
		Object myObj = server.putObject("boolean1", myBool1);
		assertNull( myObj );

		myObj = server.getObject("boolean1");
		assertEquals( myBool1, myObj );

		server.removeObject("boolean1");
		myObj = server.getObject("boolean1");
		assertNull( myObj );
	}

	@Test
	public void testByte()
		throws Exception
	{

		Object myObj = server.putObject("byte1", myByte1);
		assertNull( myObj );

		myObj = server.getObject("byte1");
		assertEquals( myByte1, myObj );

		server.removeObject("byte1");
		myObj = server.getObject("byte1");
		assertNull( myObj );
	}

	@Test
	public void testShort()
		throws Exception
	{

		Object myObj = server.putObject("short1", myShort1);
		assertNull( myObj );

		myObj = server.getObject("short1");
		assertEquals( myShort1, myObj );

		server.removeObject("short1");
		myObj = server.getObject("short1");
		assertNull( myObj );
	}

	@Test
	public void testInt()
		throws Exception
	{

		Object myObj = server.putObject("int1", myInt1);
		assertNull( myObj );

		myObj = server.getObject("int1");
		assertEquals( myInt1, myObj );

		server.removeObject("int1");
		myObj = server.getObject("int1");
		assertNull( myObj );
	}

	@Test
	public void testLong()
		throws Exception
	{

		Object myObj = server.putObject("long1", myLong1);
		assertNull( myObj );

		myObj = server.getObject("long1");
		assertEquals( myLong1, myObj );

		server.removeObject("long1");
		myObj = server.getObject("long1");
		assertNull( myObj );
	}

	@Test
	public void testFloat()
		throws Exception
	{

		Object myObj = server.putObject("float1", myFloat1);
		assertNull( myObj );

		myObj = server.getObject("float1");
		assertEquals( myFloat1, myObj );

		server.removeObject("float1");
		myObj = server.getObject("float1");
		assertNull( myObj );
	}

	@Test
	public void testDouble()
		throws Exception
	{

		Object myObj = server.putObject("double1", myDouble1);
		assertNull( myObj );

		myObj = server.getObject("double1");
		assertEquals( myDouble1, myObj );

		server.removeObject("double1");
		myObj = server.getObject("double1");
		assertNull( myObj );
	}

	@Test
	public void testString()
		throws Exception
	{

		Object myObj = server.putObject("string1", myString1);
		assertNull( myObj );

		myObj = server.getObject("string1");
		assertEquals( myString1, myObj );

		server.removeObject("string1");
		myObj = server.getObject("string1");
		assertNull( myObj );
	}



	@Test
	public void testGetAll()
		throws Exception
	{

		int size = server.size();
		assertEquals(size, 0);

		server.putObject("string1", "abc");
		server.putObject("string2", "xyz");
		server.putObject("int1", 0);
		server.putObject("int2", 125);

		int counter = 0;
		for(Entry entry: server.getAll())
		{
			if(entry.key != null && entry.value != null)
			{
				counter++;
			}
		}

		assertEquals(counter, 4);

		server.removeObject("string1");
		server.removeObject("string2");
		server.removeObject("int1");
		server.removeObject("int2");

		size = server.size();
		assertEquals(size, 0);
	}
}
