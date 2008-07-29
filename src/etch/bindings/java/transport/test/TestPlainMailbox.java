package etch.bindings.java.transport.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import java.util.ArrayList;
import java.util.List;
import java.util.Set;

import org.junit.Test;

import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.Mailbox;
import etch.bindings.java.support.Mailbox.Element;
import etch.bindings.java.support.Mailbox.Notify;
import etch.bindings.java.transport.MailboxManager;
import etch.bindings.java.transport.PlainMailbox;
import etch.bindings.java.transport.SessionMessage;
import etch.util.core.Who;

/** Test of PlainMailbox */
public class TestPlainMailbox
{
	private final MyMailboxManager mmgr = new MyMailboxManager();
	
	private final MyNotify notify = new MyNotify();
	
	/** @throws Exception */
	@Test
	public void construct1() throws Exception
	{
		testConstruct( 1L, 0, 0, 1 );

		// variations on maxMessageDelay.
		testConstruct( 2L, 1, 0, 1 );
		testConstruct( 3L, -1, 0, 1 );
		
		// variations on lifetime.
		testConstruct( 4L, 0, 1, 1 );
		
		// variations on maxMessages.
		testConstruct( 5L, 0, 0, 2 );
	}

	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void construct2() throws Exception
	{
		new PlainMailbox( null, 1L, 0, 0, 1 );
	}
	
	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void construct3() throws Exception
	{
		new PlainMailbox( mmgr, null, 0, 0, 1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct4() throws Exception
	{
		new PlainMailbox( mmgr, 1L, 0, -1, 1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct5() throws Exception
	{
		new PlainMailbox( mmgr, 1L, 0, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void closeDelivery1() throws Exception
	{
		// open mailbox and close it for delivery while empty.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void closeDelivery2() throws Exception
	{
		// open mailbox and close it for delivery while not empty.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, foo_msg );
		checkMailbox( mb, false, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, false, false, true, true, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void closeDelivery3() throws Exception
	{
		// open mailbox and close it for delivery twice.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );
		
		checkCloseDelivery( mb, false );
		checkMailbox( mb, true, false, true, true, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void closeRead1() throws Exception
	{
		// open mailbox and close it for reading while empty.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseRead( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void closeRead2() throws Exception
	{
		// open mailbox and close it for reading while not empty.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, foo_msg );
		checkMailbox( mb, false, false, false, false, 0 );
		
		checkCloseRead( mb, true );
		checkMailbox( mb, true, false, true, true, 1 );
		checkRedelivered( 0, alice_who, foo_msg );
	}
	
	/** @throws Exception */
	@Test
	public void closeRead3() throws Exception
	{
		// open mailbox and close it for reading twice while empty.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseRead( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );
		
		checkCloseRead( mb, false );
		checkMailbox( mb, true, false, true, true, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void closeRead4() throws Exception
	{
		// open mailbox and close it for reading twice while not empty.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, foo_msg );
		checkMailbox( mb, false, false, false, false, 0 );
		
		checkCloseRead( mb, true );
		checkMailbox( mb, true, false, true, true, 1 );
		checkRedelivered( 0, alice_who, foo_msg );
		
		checkCloseRead( mb, false );
		checkMailbox( mb, true, false, true, true, 1 );
		checkRedelivered( 0, alice_who, foo_msg );
	}
	
	/** @throws Exception */
	@Test
	public void full1() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, fred_who, foo_msg );
		checkMailbox( mb, false, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, bar_msg );
		checkMailbox( mb, false, true, false, false, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void full2() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, fred_who, foo_msg );
		checkMailbox( mb, false, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, bar_msg );
		checkMailbox( mb, false, true, false, false, 0 );
		
		checkDeliver( mb, false, fred_who, bar_msg );
		checkMailbox( mb, false, true, false, false, 0 );
	}

	/** @throws Exception */
	@Test
	public void read1() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, bar_msg );
		checkMailbox( mb, false, false, false, false, 0 );

		checkRead( mb, true, alice_who, bar_msg );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test
	public void read2() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, bar_msg );
		checkMailbox( mb, false, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, false, false, true, true, 0 );

		checkRead( mb, true, alice_who, bar_msg );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test
	public void read3() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, bar_msg );
		checkMailbox( mb, false, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, false, false, true, true, 0 );

		checkRead( mb, true, alice_who, bar_msg );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test
	public void read4() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );

		checkRead( mb, false, null, null );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test
	public void read5() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 20, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		Thread.sleep( 200 );

		checkRead( mb, false, null, null );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test
	public void read6() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 100, 2 );
		checkMailbox( mb, true, false, false, false, 0 );

		checkRead( mb, false, null, null );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test
	public void read7() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );

		checkRead( mb, -1, false, null, null );
		checkMailbox( mb, true, false, false, false, 0 );
	}

	/** @throws Exception */
	@Test
	public void read8() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );

		checkRead( mb, 1, false, null, null );
		checkMailbox( mb, true, false, false, false, 0 );
	}

	/** @throws Exception */
	@Test
	public void deliver1() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );
		
		checkDeliver( mb, false, fred_who, bar_msg );
		checkMailbox( mb, true, false, true, true, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void notify1() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		notify.checkMailboxStatus( false, null, null, false );
		
		Object state = new Object();
		
		mb.registerNotify( notify, state, 0 );
		notify.checkMailboxStatus( false, null, null, false );
		
		checkCloseDelivery( mb, true );
		notify.checkMailboxStatus( true, mb, state, true );
	}
	
	/** @throws Exception */
	@Test
	public void notify2() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		notify.checkMailboxStatus( false, null, null, false );
		
		Object state = new Object();
		
		mb.registerNotify( notify, state, 1000 );
		notify.checkMailboxStatus( false, null, null, false );
		
		Thread.sleep( 2000 );
		notify.checkMailboxStatus( true, mb, state, true );
	}
	
	/** @throws Exception */
	@Test
	public void notify3() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		notify.checkMailboxStatus( false, null, null, false );
		
		Object state = new Object();
		
		mb.registerNotify( notify, state, 0 );
		notify.checkMailboxStatus( false, null, null, false );
		
		Thread.sleep( 2000 );
		notify.checkMailboxStatus( false, null, null, false );
	}
	
	/** @throws Exception */
	@Test
	public void notify4() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L, -1, 0, 2 );
		notify.checkMailboxStatus( false, null, null, false );
		
		Object state = new Object();
		
		mb.registerNotify( notify, state, 0 );
		notify.checkMailboxStatus( false, null, null, false );
		
		mb.message( alice_who, foo_msg );
		notify.checkMailboxStatus( true, mb, state, false );
	}

	///////////////////
	// HELPFUL STUFF //
	///////////////////
	
	private final ValueFactory vf = new MyValueFactory();
	
	private final Type mt_foo = new Type( "foo" );
	
	private final Message foo_msg = new Message( mt_foo, vf );
	
	private final Who fred_who = new Who() {};
	
	private final Type mt_bar = new Type( "bar" );
	
	private final Message bar_msg = new Message( mt_bar, vf );
	
	private final Who alice_who = new Who() {};

	private void testConstruct( Long messageId, int maxMessageDelay, int lifetime,
		int maxMessages )
	{
		PlainMailbox mb = new PlainMailbox( mmgr, messageId, maxMessageDelay,
			lifetime, maxMessages );
		
		assertEquals( mmgr, mb.getMailboxManager() );
		assertEquals( messageId, mb.getMessageId() );
		assertEquals( maxMessageDelay, mb.getMaxMessageDelay() );
		assertEquals( lifetime, mb.getLifetime() );
		assertEquals( maxMessages, mb.getMaxMessages() );
	}
	
	private void checkDeliver( PlainMailbox mb, boolean handled, Who who,
		Message msg ) throws Exception
	{
		assertEquals( handled, mb.message( who, msg ) );
	}
	
	private void checkRead( PlainMailbox mb, boolean present, Who who, Message msg )
		throws InterruptedException
	{
		Element e = mb.read();
		if (present)
			checkElement( e, who, msg );
		else
			assertNull( e );
	}
	
	private void checkRead( PlainMailbox mb, int maxDelay, boolean present,
		Who who, Message msg ) throws InterruptedException
	{
		Element e = mb.read( maxDelay );
		if (present)
			checkElement( e, who, msg );
		else
			assertNull( e );
	}

	private void checkRedelivered( int index, Who who, Message msg )
	{
		Element e = mmgr.redelivered.get( index );
		checkElement( e, who, msg );
	}

	private void checkElement( Element e, Who who, Message msg )
	{
		assertNotNull( e );
		assertSame( who, e.sender );
		assertSame( msg, e.msg );
	}

	private void checkMailbox( PlainMailbox mb, boolean empty, boolean full,
		boolean closed, boolean unreg, int size )
	{
		assertEquals( empty, mb.isEmpty() );
		assertEquals( full, mb.isFull() );
		assertEquals( closed, mb.isClosed() );
		assertEquals( unreg, mmgr.unregistered );
		assertEquals( size, mmgr.redelivered.size() );
	}
	
	private void checkCloseRead( PlainMailbox mb, boolean closed )
		throws Exception
	{
		assertEquals( closed, mb.closeRead() );
	}
	
	private void checkCloseDelivery( PlainMailbox mb, boolean closed )
		throws Exception
	{
		assertEquals( closed, mb.closeDelivery() );
	}
	
	static class MyValueFactory implements ValueFactory
	{
		public StructValue exportCustomValue( Object value )
			throws UnsupportedOperationException
		{
			throw new UnsupportedOperationException();
		}

		public Type getCustomStructType( Class<?> c )
			throws UnsupportedOperationException
		{
			throw new UnsupportedOperationException();
		}

		public Long getInReplyTo( Message msg )
		{
			throw new UnsupportedOperationException();
		}

		public Long getMessageId( Message msg )
		{
			throw new UnsupportedOperationException();
		}

		public String getStringEncoding()
		{
			throw new UnsupportedOperationException();
		}

		public Type getType( Integer id )
		{
			throw new UnsupportedOperationException();
		}

		public Type getType( String name )
		{
			throw new UnsupportedOperationException();
		}

		public Set<Type> getTypes()
		{
			throw new UnsupportedOperationException();
		}

		public Type get_mt__exception()
		{
			throw new UnsupportedOperationException();
		}

		public Object importCustomValue( StructValue struct )
			throws UnsupportedOperationException
		{
			throw new UnsupportedOperationException();
		}

		public void setInReplyTo( Message msg, Long msgid )
		{
			throw new UnsupportedOperationException();
		}

		public void setMessageId( Message msg, Long msgid )
		{
			throw new UnsupportedOperationException();
		}
	}
	
	class MyNotify implements Notify
	{
		public void mailboxStatus( Mailbox mb, Object state, boolean closed )
		{
			mailboxStatus = true;
			this.mailbox = mb;
			this.state = state;
			this.closed = closed;
		}
		
		/**
		 * @param mailboxStatus
		 * @param mailbox
		 * @param state
		 * @param closed
		 */
		public void checkMailboxStatus( boolean mailboxStatus, Mailbox mailbox,
			Object state, boolean closed )
		{
			assertEquals( mailboxStatus, this.mailboxStatus );
			assertSame( mailbox, this.mailbox );
			assertSame( state, this.state );
			assertEquals( closed, this.closed );
		}
		
		private boolean mailboxStatus;
		
		private Mailbox mailbox;
		
		private Object state;
		
		private boolean closed;
	}
	
	class MyMailboxManager implements MailboxManager
	{
		/** */
		public final List<Element> redelivered = new ArrayList<Element>();
		
		/** */
		public boolean unregistered;
		
		public void redeliver( Who sender, Message msg ) throws Exception
		{
			redelivered.add( new Element( sender, msg ) );
		}

		public Mailbox transportCall( Who recipient, Message msg )
			throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void unregister( Mailbox mb )
		{
			unregistered = true;
		}

		public boolean sessionMessage( Who sender, Message msg )
			throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void sessionControl( Object control, Object value )
			throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void sessionNotify( Object event ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void transportMessage( Who recipient, Message msg )
			throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public SessionMessage getSession()
		{
			throw new UnsupportedOperationException();
		}

		public void setSession( SessionMessage session )
		{
			throw new UnsupportedOperationException();
		}

		public void transportControl( Object control, Object value )
			throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void transportNotify( Object event ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public Object transportQuery( Object query ) throws Exception
		{
			throw new UnsupportedOperationException();
		}
	}
}
