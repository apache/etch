package etch.bindings.java.transport.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import metreos.core.Who;
import metreos.core.io.PacketHandler;
import metreos.core.io.PacketSource;
import metreos.util.FlexBuffer;

import org.junit.Test;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator_int;
import etch.bindings.java.msg.Validator_long;
import etch.bindings.java.support.DefaultValueFactory;
import etch.bindings.java.transport.MessageHandler;
import etch.bindings.java.transport.Messagizer;
import etch.bindings.java.transport.MessagizerMessageSource;
import etch.bindings.java.transport.fmt.TypeCode;
import etch.bindings.java.transport.fmt.binary.BinaryTaggedData;

/** */
public class TestMessagizer
{
	/** */
	public TestMessagizer()
	{
		vf = new DefaultValueFactory();
		
		vf.addType( mt_add );
		vf.addType( mt_add_result );
		vf.addField( mf_x );
		vf.addField( mf_y );
		vf.addField( mf_result );
		
		mt_add.putValidator( mf_x, Validator_int.get( 0 ) );
		mt_add.putValidator( mf_y, Validator_int.get( 0 ) );
		mt_add.putValidator( DefaultValueFactory._mf__messageId, Validator_long
		    .get( 0 ) );

		mt_add_result.putValidator( mf_result, Validator_int.get( 0 ) );
		mt_add_result.putValidator( DefaultValueFactory._mf__messageId,
		    Validator_long.get( 0 ) );
		mt_add_result.putValidator( DefaultValueFactory._mf__inReplyTo,
		    Validator_long.get( 0 ) );

		ps = new MyPacketSource();
		mh = new MyMessageHandler();
		
		m = new Messagizer( mh, vf );
		m.setSource( ps );
	}

	private Type mt_add = new Type( 1, "add" );

	private Type mt_add_result = new Type( 2, "add_result" );

	private Field mf_x = new Field( 3, "x" );

	private Field mf_y = new Field( 4, "y" );

	private Field mf_result = new Field( 5, "xresult" );

	private DefaultValueFactory vf;

	private MyPacketSource ps;

	private MyMessageHandler mh;

	private Messagizer m;

	////////////////////////////////
	// Test PacketHandler methods //
	////////////////////////////////

//	/** @throws Exception */
//	@Test
//	public void up() throws Exception
//	{
//		PacketHandler ph = m;
//		
//		assertNull( mh.what );
//		assertNull( mh.src );
//		
//		ph.up( ps );
//		
//		assertSame( What.TESTMESSAGEHANDLERUP, mh.what );
//		assertSame( m, mh.src );
//		assertSame( ps, m.getSource() );
//	}
	
	private final Who who = new Who() { /* nothing */ };

	/** @throws Exception */
	@Test
	public void packet1() throws Exception
	{
		PacketHandler ph = m;
		
		FlexBuffer buf = new FlexBuffer( new byte[] { BinaryTaggedData.VERSION, TypeCode.INT1, 1, TypeCode.NONE } );
		
		mh.handled = true;
		
		ph.packet( who, buf );
		
		assertSame( What.TESTMESSAGEHANDLERMESSAGE, mh.what );
		assertSame( ps, m.getSource() );
		assertSame( who, mh.xsender );
		assertNotNull( mh.xmsg );
		assertSame( mt_add, mh.xmsg.type() );
		assertEquals( 0, mh.xmsg.size() );
	}

	/** @throws Exception */
	@Test
	public void packet2() throws Exception
	{
		PacketHandler ph = m;
		
		FlexBuffer buf = new FlexBuffer( new byte[] { BinaryTaggedData.VERSION, TypeCode.INT1, 1, TypeCode.NONE } );
		mh.handled = false;
		
		ph.packet( who, buf );
		
		assertSame( What.TESTMESSAGEHANDLERMESSAGE, mh.what );
		assertSame( ps, m.getSource() );
		assertSame( who, mh.xsender );
		assertNotNull( mh.xmsg );
		assertSame( mt_add, mh.xmsg.type() );
		assertEquals( 0, mh.xmsg.size() );
	}

	/** @throws Exception */
	@Test
	public void packet3() throws Exception
	{
		PacketHandler ph = m;
		
		FlexBuffer buf = new FlexBuffer( new byte[] { BinaryTaggedData.VERSION, TypeCode.INT1, 2, TypeCode.NONE } );
		
		mh.handled = true;
		
		ph.packet( who, buf );
		
		assertSame( What.TESTMESSAGEHANDLERMESSAGE, mh.what );
		assertSame( ps, m.getSource() );
		assertSame( who, mh.xsender );
		assertNotNull( mh.xmsg );
		assertSame( mt_add_result, mh.xmsg.type() );
		assertEquals( 0, mh.xmsg.size() );
	}

	/** @throws Exception */
	@Test
	public void packet4() throws Exception
	{
		PacketHandler ph = m;
		
		FlexBuffer buf = new FlexBuffer( new byte[] { BinaryTaggedData.VERSION, TypeCode.INT1, 2, TypeCode.NONE } );
		
		mh.handled = false;
		
		ph.packet( who, buf );
		
		assertSame( What.TESTMESSAGEHANDLERMESSAGE, mh.what );
		assertSame( ps, m.getSource() );
		assertSame( who, mh.xsender );
		assertNotNull( mh.xmsg );
		assertSame( mt_add_result, mh.xmsg.type() );
		assertEquals( 0, mh.xmsg.size() );
	}

//	/** @throws Exception */
//	@Test
//	public void down() throws Exception
//	{
//		PacketHandler ph = m;
//		
//		assertNull( mh.what );
//		assertNull( mh.src );
//		
//		ph.down( ps );
//		
//		assertSame( What.TESTMESSAGEHANDLERDOWN, mh.what );
//		assertSame( m, mh.src );
//		assertSame( ps, m.getSource() );
//	}

	//////////////////////////////////////////
	// Test MessagizerMessageSource methods //
	//////////////////////////////////////////

//	/** @throws Exception */
//	@Test
//	public void close1() throws Exception
//	{
//		MessagizerMessageSource mms = m;
//		
//		assertNull( ps.what );
//		
//		mms.close( true );
//		
//		assertSame( What.PACKETSOURCECLOSE, ps.what );
//		assertTrue( ps.reset );
//	}

//	/** @throws Exception */
//	@Test
//	public void close2() throws Exception
//	{
//		MessagizerMessageSource mms = m;
//		
//		assertNull( ps.what );
//		
//		mms.close( false );
//		
//		assertSame( What.PACKETSOURCECLOSE, ps.what );
//		assertFalse( ps.reset );
//	}

//	/** @throws Exception */
//	@Test
//	public void localAddress() throws Exception
//	{
//		MessagizerMessageSource mms = m;
//		SocketAddress isa = new InetSocketAddress( 23 );
//		
//		ps.localAddress = isa;
//		
//		assertSame( isa, mms.localAddress() );
//	}

//	/** @throws Exception */
//	@Test
//	public void remoteAddress() throws Exception
//	{
//		MessagizerMessageSource mms = m;
//		SocketAddress isa = new InetSocketAddress( 23 );
//		
//		ps.remoteAddress = isa;
//		
//		assertSame( isa, mms.remoteAddress() );
//	}

//	/** @throws Exception */
//	@Test
//	public void shutdownInput() throws Exception
//	{
//		MessagizerMessageSource mms = m;
//		
//		assertNull( ps.what );
//		
//		mms.shutdownInput();
//		
//		assertSame( What.PACKETSOURCESHUTDOWNINPUT, ps.what );
//	}

//	/** @throws Exception */
//	@Test
//	public void shutdownOutput() throws Exception
//	{
//		MessagizerMessageSource mms = m;
//		
//		assertNull( ps.what );
//		
//		mms.shutdownOutput();
//		
//		assertSame( What.PACKETSOURCESHUTDOWNOUTPUT, ps.what );
//	}

//	/** @throws Exception */
//	@Test
//	public void stop() throws Exception
//	{
//		MessagizerMessageSource mms = m;
//		
//		assertNull( ps.what );
//		
//		mms.stop();
//		
//		assertSame( What.PACKETSOURCESTOP, ps.what );
//	}

	/** @throws Exception */
	@Test
	public void message1() throws Exception
	{
		MessagizerMessageSource mms = m;
		
		Message msg = new Message( mt_add, vf );
		ps.headerSize = 0;
		
		assertNull( ps.what );
		
		mms.messagex( who, msg );

		assertSame( What.PACKETSOURCEPACKET, ps.what );
		assertSame( who, ps.xrecipient );
		assertNotNull( ps.xbuf );
		assertEquals( 3, ps.xbuf.length );
		assertEquals( BinaryTaggedData.VERSION, ps.xbuf[0] );
		assertEquals( (byte) 65, ps.xbuf[1] );
		assertEquals( TypeCode.NONE, ps.xbuf[2] );
	}

	/** @throws Exception */
	@Test
	public void message2() throws Exception
	{
		MessagizerMessageSource mms = m;
		
		Message msg = new Message( mt_add, vf );
		ps.headerSize = 8;
		
		assertNull( ps.what );
		
		mms.messagex( who, msg );

		assertSame( What.PACKETSOURCEPACKET, ps.what );
		assertSame( who, ps.xrecipient );
		assertNotNull( ps.xbuf );
		assertEquals( 11, ps.xbuf.length );
		assertEquals( (byte) 0, ps.xbuf[0] );
		assertEquals( (byte) 0, ps.xbuf[1] );
		assertEquals( (byte) 0, ps.xbuf[2] );
		assertEquals( (byte) 0, ps.xbuf[3] );
		assertEquals( (byte) 0, ps.xbuf[4] );
		assertEquals( (byte) 0, ps.xbuf[5] );
		assertEquals( (byte) 0, ps.xbuf[6] );
		assertEquals( (byte) 0, ps.xbuf[7] );
		assertEquals( BinaryTaggedData.VERSION, ps.xbuf[8] );
		assertEquals( (byte) 65, ps.xbuf[9] );
		assertEquals( TypeCode.NONE, ps.xbuf[10] );
	}

//	@Test
//	public void sendreceiveMessageIterative() throws Exception
//	{
//		report = false;
//
//		TestMessageHandler tm = new TestMessageHandler();
//		int n = 10;
//		long t0 = System.nanoTime();
//		for (int i = 0; i < n; i++)
//		{
//			ps.checkFlag = false;
//			tm.send( m, ps );
//			assertTrue( ps.checkFlag );
//		}
//		long t1 = System.nanoTime();
//		double t = (t1 - t0) / 1000000000.0;
//		System.out.printf( "took %f to send %d (%f per sec)\n", t, n, n / t );
//	}

	/**
	 * Reports activities which occur.
	 */
	public enum What
	{
		/** */
		TESTMESSAGEHANDLERMESSAGE,
		/** */
		PACKETSOURCEPACKET,
		/** */
		OOB_QUERY_SOURCE,
		/** */
		OOB_CONTROL_SOURCE,
		/** */
		OOB_NOTIFY_SOURCE,
		/** */
		OOB_QUERY_HANDLER,
		/** */
		OOB_CONTROL_HANDLER,
		/** */
		OOB_NOTIFY_HANDLER
	};

	/** */
	public static class MyPacketSource implements PacketSource
	{
		/** */
		public What what;
		/** */
		public int headerSize = 0;
		/** */
		public PacketHandler handler;
		/** */
		public boolean reset;
		/** */
		public Who xrecipient;
		/** */
		public byte[] xbuf;
		/** */
		public Object xquery;
		/** */
		public Object xquery_result;
		/** */
		public Object xcontrol;
		/** */
		public Object xvalue;
		/** */
		public Object xevent;
		
		public int headerSize()
        {
	        return headerSize;
        }

		public void packet( Who recipient, FlexBuffer buf ) throws Exception
        {
	        what = What.PACKETSOURCEPACKET;
	        this.xrecipient = recipient;
	        this.xbuf = buf.getAvailBytes();
        }
		
		public PacketHandler getHandler()
		{
			return handler;
		}

		public void setHandler( PacketHandler handler )
        {
	        this.handler = handler;
        }

		public Object transportQuery( Object query ) throws Exception
		{
			what = What.OOB_QUERY_SOURCE;
			xquery = query;
			return xquery_result;
		}

		public void transportControl( Object control, Object value ) throws Exception
		{
			what = What.OOB_CONTROL_SOURCE;
			xcontrol = control;
			xvalue = value;
		}

		public void transportNotify( Object event ) throws Exception
		{
			what = What.OOB_NOTIFY_SOURCE;
			xevent = event;
		}
	}

	/** */
	public static class MyMessageHandler implements
	    MessageHandler<MessagizerMessageSource>
	{
		/** */
		public What what;
		/** */
		public Who xsender;
		/** */
		public Message xmsg;
		/** */
		public boolean handled;
		/** */
		public Object xquery;
		/** */
		public Object xquery_result;
		/** */
		public Object xcontrol;
		/** */
		public Object xvalue;
		/** */
		public Object xevent;

		public boolean message( Who sender, Message msg ) throws Exception
		{
			what = What.TESTMESSAGEHANDLERMESSAGE;
			this.xsender = sender;
			this.xmsg = msg;
			return handled;
		}

		public MessagizerMessageSource getSource()
		{
			// ignore.
			return null;
		}

		public void setSource( MessagizerMessageSource src )
		{
			// ignore.
		}

		public Object sessionQuery( Object query )
		{
			what = What.OOB_QUERY_HANDLER;
			xquery = query;
			return xquery_result;
		}

		public void sessionControl( Object control, Object value )
		{
			what = What.OOB_CONTROL_HANDLER;
			xcontrol = control;
			xvalue = value;
		}

		public void sessionNotify( Object event )
		{
			what = What.OOB_NOTIFY_HANDLER;
			xevent = event;
		}
	}
}
