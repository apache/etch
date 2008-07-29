// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package etch.bindings.java.transport;

import etch.bindings.java.msg.Message;
import etch.bindings.java.support.Mailbox;
import etch.util.AlarmListener;
import etch.util.AlarmManager;
import etch.util.CircularQueue;
import etch.util.core.Who;

/**
 * A plain implementation of a mailbox using a fixed size circular queue.
 */
public class PlainMailbox implements Mailbox, AlarmListener
{
	/**
	 * Constructs the Mailbox.
	 *
	 * @param mailboxManager the mailbox manager to use to unregister this mailbox
	 * and to deliver undelivered messages.
	 * @param messageId
	 * @param maxMessageDelay the maximum delay in milliseconds to
	 * wait when trying to put a message in the mailbox.
	 * @param lifetime time in milliseconds to keep the mailbox open.
	 * Lifetime of 0 means keep it open until explicitly closed.
	 * @param maxMessages 
	 */
	public PlainMailbox( MailboxManager mailboxManager, Long messageId,
		int maxMessageDelay, int lifetime, int maxMessages )
	{
		if (mailboxManager == null)
			throw new NullPointerException( "mailboxManager == null" );
		
		if (messageId == null)
			throw new NullPointerException( "messageId == null" );
		
		if (lifetime < 0)
			throw new IllegalArgumentException( "lifetime < 0" );
		
		if (maxMessages < 1)
			throw new IllegalArgumentException( "maxMessages < 1" );
		
		this.mailboxManager = mailboxManager;
		this.messageId = messageId;
		this.maxMessageDelay = maxMessageDelay;
		this.lifetime = lifetime;
		this.maxMessages = maxMessages;
		
		queue = new CircularQueue<Mailbox.Element>( maxMessages );
		
		if (lifetime > 0)
		{
			alarmSet = true;
			AlarmManager.staticAdd( this, null, lifetime );
		}
	}
	
	private final MailboxManager mailboxManager;
	
	private final Long messageId;
	
	private final int maxMessageDelay;
	
	private final int lifetime;
	
	private final int maxMessages;
	
	private boolean alarmSet;
	
	private final CircularQueue<Mailbox.Element> queue;

	/**
	 * @return the mailboxManager of this mailbox.
	 */
	public MailboxManager getMailboxManager()
	{
		return mailboxManager;
	}

	public Long getMessageId()
	{
		return messageId;
	}

	/**
	 * @return the maxMessageDelay of this mailbox.
	 */
	public int getMaxMessageDelay()
	{
		return maxMessageDelay;
	}

	/**
	 * @return the lifetime of this mailbox.
	 */
	public int getLifetime()
	{
		return lifetime;
	}

	/**
	 * @return the maxMessages of this mailbox.
	 */
	public int getMaxMessages()
	{
		return maxMessages;
	}

	public boolean message( Who sender, Message msg ) throws Exception
	{
		boolean ok = queue.put( new Mailbox.Element( sender, msg ), maxMessageDelay );
		if (ok)
			fireNotify();
		return ok;
	}
	
	private void fireNotify()
	{
		Notify n;
		Object s;
		boolean c;
		
		synchronized (queue)
		{
			n = notify;
			s = state;
			c = queue.isClosed();
		}
		
		if (n != null)
			n.mailboxStatus( this, s, c );
	}

	public Mailbox.Element read() throws InterruptedException
	{
		return queue.get();
	}
	
	public Mailbox.Element read( int maxDelay ) throws InterruptedException
	{
		return queue.get( maxDelay );
	}
	
	public boolean closeDelivery()
	{
		synchronized (queue)
		{
			if (queue.isClosed())
				return false;

			if (alarmSet)
			{
				alarmSet = false;
				AlarmManager.staticRemove( this );
			}
			
			mailboxManager.unregister( this );
			queue.close();
		}
		
		fireNotify();
		return true;
	}

	public boolean closeRead() throws Exception
	{
		if (closeDelivery())
		{
			Mailbox.Element mbe;
			while ((mbe = read()) != null)
				mailboxManager.redeliver( mbe.sender, mbe.msg );
			return true;
		}
		return false;
	}

	public int wakeup( AlarmManager manager, Object state, long due )
	{
		closeDelivery();
		return 0;
	}

	public void registerNotify( Notify notify, Object state, int maxDelay )
	{
		if (notify == null)
			throw new NullPointerException( "notify == null" );
		
		if (maxDelay < 0)
			throw new IllegalArgumentException( "maxDelay < 0" );
		
		if (this.notify != null)
			throw new IllegalStateException( "this.notify != null" );
		
		boolean isNotEmptyOrIsClosed;
		synchronized (queue)
		{
			this.notify = notify;
			this.state = state;
			
			if (maxDelay > 0)
			{
				alarmSet = true;
				AlarmManager.staticAdd( this, null, maxDelay );
			}
			
			isNotEmptyOrIsClosed = !queue.isEmpty() || queue.isClosed();
		}
		if (isNotEmptyOrIsClosed)
			fireNotify();
	}

	public void unregisterNotify( Notify notify )
	{
		if (notify == null)
			throw new NullPointerException( "notify == null" );
		
		if (notify != this.notify)
			throw new IllegalStateException( "notify != this.notify" );
		
		synchronized (queue)
		{
			if (alarmSet)
			{
				alarmSet = false;
				AlarmManager.staticRemove( this );
			}
			
			this.notify = null;
			this.state = null;
		}
	}
	
	private Notify notify;
	
	private Object state;

	public boolean isEmpty()
	{
		return queue.isEmpty();
	}
	
	public boolean isClosed()
	{
		return queue.isClosed();
	}

	public boolean isFull()
	{
		return queue.isFull();
	}
}
