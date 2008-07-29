package etch.bindings.java.support.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.fail;

import org.junit.Test;

import metreos.core.Who;
import etch.bindings.java.msg.Message;
import etch.bindings.java.support.MessageSource;
import etch.bindings.java.support.StubBase;
import etch.bindings.java.support.Unwanted;

/** Test StubBase */
public class TestStubBase
{
	/** */
	@Test
	public void nothing()
	{
		// nothing to do.
	}
	
    /** test events */
	enum What
	{ 
	    /** wanted message delivered */ UNWANTED
	};
	
    /** @throws Exception */
	// TODO @Test
	public void unwanted2() throws Exception
	{
		MyUnwanted obj = new MyUnwanted();
		MessageSource src = new MyMessageSource();
		StubBase sb = new StubBase( src, obj, null, null );
		
		Who sender = new Who() { /* nothing */ };
		Message msg = new Message( null, null );
		
		sb.message( sender, msg );
		
		assertEquals( obj.what, What.UNWANTED );
		assertSame( sender, obj.xsender );
		assertSame( msg, obj.xmsg );
	}

    /** @throws Exception */
	// TODO @Test( expected=UnsupportedOperationException.class )
	public void unwanted4() throws Exception
	{
		Object obj = new Object();
		MessageSource src = new MyMessageSource();
		StubBase sb = new StubBase( src, obj, null, null );
		
		Who sender = new Who() { /* nothing */ };
		Message msg = new Message( null, null );
		
		sb.message( sender, msg );
		fail( "delivery of unwanted message should fail" );
	}
	
	/**
	 * MessageSource for testing.
	 */
	public static class MyMessageSource implements MessageSource
	{
		public void messagex( Who recipient, Message msg ) throws Exception
		{
			fail( "not needed" );
		}
	}
	
	/**
	 * Unwanted handler for testing.
	 */
	public static class MyUnwanted implements Unwanted
	{
		public void _unwanted( Who sender, Message msg )
		{
			assertNull( what );
			what = What.UNWANTED;
			this.xsender = sender;
			this.xmsg = msg;
		}
		
		/** what event */
		public What what;
		
		/** sender of the unwanted message */
		public Who xsender;
		
		/** unwanted message */
		public Message xmsg;
	}
}
