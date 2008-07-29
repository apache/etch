package etch.bindings.java.support.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import org.junit.Test;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.DefaultValueFactory;
import etch.bindings.java.support.DeliveryService;
import etch.bindings.java.support.Mailbox;
import etch.bindings.java.support.RemoteBase;
import etch.bindings.java.transport.PlainMailbox;

/**
 * Test of RemoteBase.
 */
public class TestRemoteBase
{
	private MyDeliveryService svc = new MyDeliveryService();
	private DefaultValueFactory vf = new DefaultValueFactory();
	private RemoteBase<DefaultValueFactory> rb = new RemoteBase<DefaultValueFactory>( svc, vf );
	private Type mt = new Type( "foo" );
	private Type rmt = new Type( "bar" );
	private Field rmf = new Field( "baz" );
	
	/** @throws Exception */
	@Test
	public void newMessage() throws Exception
	{
		Message msg = rb._newMessage( mt );
		msg.checkType( mt );
		assertSame( vf, msg.vf() );
		assertNull( svc.what );
		assertNull( svc.xmsg );
		assertNull( svc.xresponseType );
		assertNull( svc.xresponseField );
		assertNull( svc.xtimeout );
	}

	/** @throws Exception */
	@Test
	public void send() throws Exception
	{
		Message msg = rb._newMessage( mt );
		
		rb._svc.send( msg );
		
		assertSame( svc.what, What.SEND );
		assertSame( svc.xmsg, msg );
		assertNull( svc.xresponseType );
		assertNull( svc.xresponseField );
		assertNull( svc.xtimeout );
	}

	/** @throws Exception */
	@Test
	public void voidcall() throws Exception
	{
		Message msg = rb._newMessage( mt );
		
		rb._svc.endvoidcall( rb._svc.begincall( msg ), rmt, rmf, 99 );
		
		assertSame( svc.what, What.VOIDCALL );
		assertSame( svc.xmsg, msg );
		assertSame( svc.xresponseType, rmt );
		assertSame( svc.xresponseField, rmf );
		assertEquals( svc.xtimeout, 99 );
	}

	/** @throws Exception */
	@Test
	public void call() throws Exception
	{
		Message msg = rb._newMessage( mt );
		
		Object result = rb._svc.endcall( rb._svc.begincall( msg ), rmt, rmf, 98 );
		
		assertSame( svc.what, What.CALL );
		assertSame( svc.xmsg, msg );
		assertSame( svc.xresponseType, rmt );
		assertSame( svc.xresponseField, rmf );
		assertEquals( svc.xtimeout, 98 );
		assertEquals( result, 23 );
	}
	
	/**
	 * testing events.
	 */
	enum What
	{
		/** send was called */
		SEND,
		/** voidcall was called */
		VOIDCALL,
		/** call was called */
		CALL,
		/** begincall was called */
		BEGINCALL
	};
	
	/**
	 * DeliveryService for testing.
	 */
	public static class MyDeliveryService implements DeliveryService
	{
		/** event */
		public What what;
		/** message sent */
		public Message xmsg;
		/** type of response received */
		public Type xresponseType;
		/** field of response received */
		public Field xresponseField;
		/** timeout received */
		public Integer xtimeout;
		/** mailbox to return */
		public Mailbox xmb;
		
		public void send( Message msg ) throws Exception
		{
			assertNull( what );
			what = What.SEND;
			this.xmsg = msg;
		}
		
		public Mailbox begincall( Message msg )
		{
			assertNull( what );
			what = What.BEGINCALL;
			this.xmsg = msg;
			this.xmb = new PlainMailbox( null, 0L, 0, 0, 1 );
			return this.xmb;
		}

		public void endvoidcall( Mailbox mb, Type responseType,
			Field responseField, int timeout ) throws Exception
		{
			assertSame( What.BEGINCALL, what );
			assertSame( xmb, mb );
			what = What.VOIDCALL;
			xmb = null;
			this.xresponseType = responseType;
			this.xresponseField = responseField;
			this.xtimeout = timeout;
		}
		
		public Object endcall( Mailbox mb, Type responseType,
			Field responseField, int timeout ) throws Exception
		{
			assertSame( What.BEGINCALL, what );
			assertSame( xmb, mb );
			what = What.CALL;
			xmb = null;
			this.xresponseType = responseType;
			this.xresponseField = responseField;
			this.xtimeout = timeout;
			return 23;
		}

		public void transportControl( Object control, Object value ) throws Exception
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
}
