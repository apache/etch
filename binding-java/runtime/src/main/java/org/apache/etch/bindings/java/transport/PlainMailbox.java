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

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.support.Mailbox;
import org.apache.etch.util.AlarmListener;
import org.apache.etch.util.AlarmManager;
import org.apache.etch.util.CircularQueue;
import org.apache.etch.util.core.Who;


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
	 * @param messageId message id of the message whose reply will come to this
	 * mailbox.
	 */
	public PlainMailbox( MailboxManager mailboxManager, Long messageId )
	{
		if (mailboxManager == null)
			throw new NullPointerException( "mailboxManager == null" );
		
		if (messageId == null)
			throw new NullPointerException( "messageId == null" );
		
		this.mailboxManager = mailboxManager;
		this.messageId = messageId;
		
		queue = new CircularQueue<Mailbox.Element>( 1 );
	}
	
	private final MailboxManager mailboxManager;
	
	private final Long messageId;
	
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

	public boolean message( Who sender, Message msg ) throws Exception
	{
		boolean ok = queue.put( new Mailbox.Element( sender, msg ), -1 );
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

	public int wakeup( AlarmManager manager, Object xstate, long due )
	{
		closeDelivery();
		return 0;
	}

	public void registerNotify( Notify newNotify, Object newState, int maxDelay )
	{
		if (newNotify == null)
			throw new NullPointerException( "newNotify == null" );
		
		if (maxDelay < 0)
			throw new IllegalArgumentException( "maxDelay < 0" );
		
		boolean isNotEmptyOrIsClosed;
		
		synchronized (queue)
		{
			if (notify != null)
				throw new IllegalStateException( "this.notify != null" );
			
			notify = newNotify;
			state = newState;
			
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

	public void unregisterNotify( Notify oldNotify )
	{
		if (oldNotify == null)
			throw new NullPointerException( "oldNotify == null" );
		
		if (notify == null)
			return;
		
		synchronized (queue)
		{
			if (oldNotify != notify)
				throw new IllegalStateException( "oldNotify != this.notify" );
			
			if (alarmSet)
			{
				alarmSet = false;
				AlarmManager.staticRemove( this );
			}
			
			notify = null;
			state = null;
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
