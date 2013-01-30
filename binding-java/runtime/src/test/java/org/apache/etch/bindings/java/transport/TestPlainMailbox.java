/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.bindings.java.transport;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import java.util.ArrayList;
import java.util.List;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.DummyValueFactory;
import org.apache.etch.bindings.java.support.Mailbox;
import org.apache.etch.bindings.java.support.Mailbox.Element;
import org.apache.etch.bindings.java.support.Mailbox.Notify;
import org.apache.etch.util.core.Who;
import org.junit.Ignore;
import org.junit.Test;


/** Test of PlainMailbox */
public class TestPlainMailbox
{
	private final MyMailboxManager mmgr = new MyMailboxManager();

	private final MyNotify notify = new MyNotify();
	private final MyNotify notify1 = new MyNotify();
	
	/** @throws Exception */
	@Test
	public void construct1() throws Exception
	{
		testConstruct( 1L );
	}

	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void construct2() throws Exception
	{
		new PlainMailbox( null, 1L );
	}
	
	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void construct3() throws Exception
	{
		new PlainMailbox( mmgr, null );
	}
	
	/** @throws Exception */
	@Test
	public void closeDelivery1() throws Exception
	{
		// open mailbox and close it for delivery while empty.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void closeDelivery2() throws Exception
	{
		// open mailbox and close it for delivery while not empty.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, foo_msg );
		checkMailbox( mb, false, true, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, false, true, true, true, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void closeDelivery3() throws Exception
	{
		// open mailbox and close it for delivery twice.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
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
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseRead( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void closeRead2() throws Exception
	{
		// open mailbox and close it for reading while not empty.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, foo_msg );
		checkMailbox( mb, false, true, false, false, 0 );
		
		checkCloseRead( mb, true );
		checkMailbox( mb, true, false, true, true, 1 );
		checkRedelivered( 0, alice_who, foo_msg );
	}
	
	/** @throws Exception */
	@Test
	public void closeRead3() throws Exception
	{
		// open mailbox and close it for reading twice while empty.
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
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
		
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, foo_msg );
		checkMailbox( mb, false, true, false, false, 0 );
		
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
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, fred_who, foo_msg );
		checkMailbox( mb, false, true, false, false, 0 );
		
		checkDeliver( mb, false, alice_who, bar_msg );
		checkMailbox( mb, false, true, false, false, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void full2() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, fred_who, foo_msg );
		checkMailbox( mb, false, true, false, false, 0 );
		
		checkDeliver( mb, false, alice_who, bar_msg );
		checkMailbox( mb, false, true, false, false, 0 );
		
		checkDeliver( mb, false, mark_who, baz_msg );
		checkMailbox( mb, false, true, false, false, 0 );
	}

	/** @throws Exception */
	@Test
	public void read1() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, bar_msg );
		checkMailbox( mb, false, true, false, false, 0 );

		checkRead( mb, true, alice_who, bar_msg );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test
	public void read2() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, bar_msg );
		checkMailbox( mb, false, true, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, false, true, true, true, 0 );

		checkRead( mb, true, alice_who, bar_msg );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test
	public void read3() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkDeliver( mb, true, alice_who, bar_msg );
		checkMailbox( mb, false, true, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, false, true, true, true, 0 );

		checkRead( mb, true, alice_who, bar_msg );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test
	public void read4() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		checkCloseDelivery( mb, true );
		checkMailbox( mb, true, false, true, true, 0 );

		checkRead( mb, false, null, null );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test @Ignore
	public void read5() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );
		
		Thread.sleep( 1000 );

		checkRead( mb, false, null, null );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test @Ignore
	public void read6() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );

		checkRead( mb, false, null, null );
		checkMailbox( mb, true, false, true, true, 0 );
	}

	/** @throws Exception */
	@Test
	public void read7() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );

		checkRead( mb, -1, false, null, null );
		checkMailbox( mb, true, false, false, false, 0 );
	}

	/** @throws Exception */
	@Test
	public void read8() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		checkMailbox( mb, true, false, false, false, 0 );

		checkRead( mb, 1, false, null, null );
		checkMailbox( mb, true, false, false, false, 0 );
	}

	/** @throws Exception */
	@Test
	public void deliver1() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
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
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
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
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
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
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
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
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		notify.checkMailboxStatus( false, null, null, false );
		
		Object state = new Object();
		
		mb.registerNotify( notify, state, 0 );
		notify.checkMailboxStatus( false, null, null, false );
		
		mb.message( alice_who, foo_msg );
		notify.checkMailboxStatus( true, mb, state, false );
	}
	
	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void reg1() throws Exception
	{
		// notify == null
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.registerNotify( null, null, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void reg2() throws Exception
	{
		// maxDelay < 0
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.registerNotify( notify, null, -1 );
	}
	
	/** @throws Exception */
	@Test
	public void reg3() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.registerNotify( notify, null, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void reg4() throws Exception
	{
		// this.notify != null
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.registerNotify( notify, null, 0 );
		mb.registerNotify( notify, null, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void reg5() throws Exception
	{
		// this.notify != null
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.registerNotify( notify, null, 0 );
		mb.registerNotify( notify1, null, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void unreg1() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.unregisterNotify( notify );
	}
	
	/** @throws Exception */
	@Test
	public void unreg2() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.registerNotify( notify, null, 0 );
		mb.unregisterNotify( notify );
	}
	
	/** @throws Exception */
	@Test
	public void unreg3() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.registerNotify( notify, null, 0 );
		mb.unregisterNotify( notify );
		mb.unregisterNotify( notify );
		mb.unregisterNotify( notify1 );
	}
	
	/** @throws Exception */
	@Test
	public void unreg4() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.registerNotify( notify, null, 0 );
		mb.unregisterNotify( notify );
		mb.registerNotify( notify, null, 0 );
		mb.unregisterNotify( notify );
		mb.registerNotify( notify1, null, 0 );
		mb.unregisterNotify( notify1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void unreg5() throws Exception
	{
		// notify != this.notify
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.registerNotify( notify, null, 0 );
		mb.unregisterNotify( notify1 );
	}
	
	/** @throws Exception */
	@Test
	public void unreg6() throws Exception
	{
		PlainMailbox mb = new PlainMailbox( mmgr, 1L );
		mb.registerNotify( notify, null, 30 );
		mb.unregisterNotify( notify );
	}

	///////////////////
	// HELPFUL STUFF //
	///////////////////
	
	private final ValueFactory vf = new DummyValueFactory();
	
	private final Type mt_foo = new Type( "foo" );
	private final Message foo_msg = new Message( mt_foo, vf );
	private final Who fred_who = new Who() { /* */ };
	
	private final Type mt_bar = new Type( "bar" );
	private final Message bar_msg = new Message( mt_bar, vf );
	private final Who alice_who = new Who() { /* */ };
	
	private final Type mt_baz = new Type( "baz" );
	private final Message baz_msg = new Message( mt_baz, vf );
	private final Who mark_who = new Who() { /* */ };

	private void testConstruct( Long messageId )
	{
		PlainMailbox mb = new PlainMailbox( mmgr, messageId );
		
		assertEquals( mmgr, mb.getMailboxManager() );
		assertEquals( messageId, mb.getMessageId() );
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
	
	/** */
	static class MyNotify implements Notify
	{
		public void mailboxStatus( Mailbox mb, Object state, boolean closed )
		{
			mailboxStatus = true;
			mailbox = mb;
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
	
	/** */
	static class MyMailboxManager implements MailboxManager
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
