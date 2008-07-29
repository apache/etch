// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package etch.bindings.java.transport;

import metreos.core.Who;
import metreos.util.AlarmListener;
import metreos.util.AlarmManager;
import metreos.util.CircularQueue;
import etch.bindings.java.msg.Message;
import etch.bindings.java.support.Mailbox;

/**
 * A plain implementation of a mailbox using a fixed size circular queue.
 */
public class PlainMailbox implements Mailbox, AlarmListener
{
	/**
	 * Constructs the Mailbox.
	 *
	 * @param mgr
	 * @param msgid
	 * @param maxMessageDelay 
	 * @param lifetime time in milliseconds to keep the mailbox open.
	 * Lifetime of 0 means keep it open until explicitly closed.
	 * @param maxMessages 
	 */
	public PlainMailbox( MailboxManager mgr, Long msgid, int maxMessageDelay,
		int lifetime, int maxMessages )
	{
		if (lifetime < 0)
			throw new IllegalArgumentException( "lifetime < 0" );
		
		if (maxMessages < 1)
			throw new IllegalArgumentException( "maxMessages < 1" );
		
		this.mgr = mgr;
		this.msgid = msgid;
		this.maxMessageDelay = maxMessageDelay;
		this.lifetime = lifetime;
		
		if (lifetime > 0)
			AlarmManager.staticAdd( this, null, lifetime );
		
		queue = new CircularQueue<Mailbox.Element>( maxMessages );
	}
	
	private final MailboxManager mgr;
	
	private final Long msgid;
	
	private final int maxMessageDelay;
	
	private final int lifetime;
	
	private final CircularQueue<Mailbox.Element> queue;

	public boolean message( Who sender, Message msg ) throws Exception
	{
		return queue.put( new Mailbox.Element( sender, msg ), maxMessageDelay );
	}
	
	public Mailbox.Element read() throws InterruptedException
	{
		return queue.get();
	}
	
	public Mailbox.Element read( int maxDelay ) throws InterruptedException
	{
		return queue.get( maxDelay );
	}
	
	public void closeDelivery()
	{
		if (lifetime > 0)
			AlarmManager.staticRemove( this );
		mgr.unregister( this );
		queue.close();
	}

	public void closeRead() throws Exception
	{
		closeDelivery();
		Mailbox.Element mbe;
		while ((mbe = read()) != null)
			mgr.redeliver( mbe.sender, mbe.msg );
	}

	public int wakeup( AlarmManager manager, Object state, long due )
	{
		closeDelivery();
		return 0;
	}

	public Long getMessageId()
	{
		return msgid;
	}
}
