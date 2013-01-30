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

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.support.Mailbox;
import org.apache.etch.util.IdGenerator;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Session;


/**
 * MailboxManager is a MessageHandler which accepts packets for
 * possible delivery to a mailbox, or to another message handler
 * if an appropriate mailbox cannot be found. MailboxManager is
 * also a MessageSource, tagging messages with msgids and then
 * forwarding them to another MessageSource. If requested, a
 * mailbox is created with a message's msgid and added to the
 * set of mailboxes waiting for messages.
 */
public class PlainMailboxManager implements MailboxManager
{
	/**
	 * @param transport used to deliver messages to our peer.
	 * @param uri
	 * @param resources
	 */
	public PlainMailboxManager( TransportMessage transport, String uri,
		Resources resources )
	{
		this( transport, new URL( uri ), resources );
	}

	/**
	 * @param transport used to deliver messages to our peer.
	 * @param uri
	 * @param resources
	 */
	public PlainMailboxManager( TransportMessage transport, URL uri,
		Resources resources )
	{
		this.transport = transport;
		transport.setSession( this );
	}
	
	private final TransportMessage transport;
	
	/**
	 * @return the transport.
	 */
	public TransportMessage getTransport()
	{
		return transport;
	}
	
	@Override
	public String toString()
	{
		return String.format( "PlainMailboxManager/%s", transport );
	}

	////////////////////////////
	// MessageHandler methods //
	////////////////////////////
	
	public boolean sessionMessage( Who sender, Message msg ) throws Exception
	{
		//Log.report( "MailboxManager.recv", "msg", msg );
		Long msgid = msg.getInReplyTo();
		if (msgid != null)
		{
			Mailbox mb = getMailbox( msgid );
			//Log.report( "MailboxManager.recv", "msg", msg, "mb", mb );
			if (mb != null)
			{
				try
				{
					return mb.message( sender, msg );
				}
				catch ( InterruptedException e )
				{
					// timeout or mailbox closed - fall through
				}
			}
			// no such mailbox - reject
			return false;
		}
		// no msgid - pass off to session
		return session.sessionMessage( sender, msg );
	}

	///////////////////////////
	// MessageSource methods //
	///////////////////////////

	public void transportMessage( Who recipient, Message msg ) throws Exception
	{
		if (msg.getMessageId() != null)
			throw new IllegalStateException( "message has already been sent" );
		
		msg.setMessageId( idGen.next() );
		
		//Log.report( "MailboxManager.send", "msg", msg );
		
		transport.transportMessage( recipient, msg );
	}

	public Mailbox transportCall( Who recipient, Message msg ) throws Exception
	{
		if (msg.getMessageId() != null)
			throw new IllegalStateException( "message has already been sent" );
		
		if (msg.getInReplyTo() != null)
			throw new IllegalStateException( "message is marked as a reply" );
		
		Long msgid = idGen.next();
		msg.setMessageId( msgid );
		
		Mailbox mb = new PlainMailbox( this, msgid );
		register( mb );
		
		//Log.report( "MailboxManager.send", "msg", msg );

		try
		{
			transport.transportMessage( recipient, msg );
		}
		catch ( Exception e )
		{
			unregister( mb );
			throw e;
		}
		
		return mb;
	}
	
	private final IdGenerator idGen = new IdGenerator( System.currentTimeMillis(), 37 );

	/////////////////////
	// Mailbox methods //
	/////////////////////
	
	/**
	 * Adds a mailbox to the set of mailbox receiving responses
	 * to messages.
	 * @param mb
	 */
	public void register( Mailbox mb )
	{
		Long msgid = mb.getMessageId();
		synchronized (mailboxes)
		{
			if (!up)
				throw new IllegalStateException( "connection down" );
			
			if (mailboxes.containsKey( msgid ))
				throw new IllegalArgumentException( "dup msgid in mailboxes" );
			
			mailboxes.put( msgid, mb );
		}
	}

	public void unregister( Mailbox mb )
	{
		mailboxes.remove( mb.getMessageId() );
	}
	
	private void unregisterAll()
	{
		for (Mailbox mb: mailboxes.values().toArray( new Mailbox[0] ))
			mb.closeDelivery();
	}
	
	/**
	 * Returns the mailbox for the specified msgid. This is a testing api.
	 * @param msgid
	 * @return the mailbox for the specified msgid.
	 */
	public Mailbox getMailbox( Long msgid )
	{
		return mailboxes.get( msgid );
	}
	
	private Map<Long, Mailbox> mailboxes = Collections.synchronizedMap(
		new HashMap<Long, Mailbox>() );

	public void redeliver( Who sender, Message msg ) throws Exception
	{
		session.sessionMessage( sender, msg );
	}

	/**
	 * @return the number of mailboxes
	 */
	public int size()
	{
		return mailboxes.size();
	}
	
	///////////////////////////
	// SourceHandler methods //
	///////////////////////////

	public Object sessionQuery( Object query ) throws Exception
	{
		return session.sessionQuery( query );
	}

	public void sessionControl( Object control, Object value ) throws Exception
	{
		session.sessionControl( control, value );
	}

	public void sessionNotify( Object event ) throws Exception
	{
		if (event == Session.UP)
		{
			up = true;
		}
		else if (event == Session.DOWN)
		{
			up = false;
			unregisterAll();
		}
		
		session.sessionNotify( event );
	}
	
	private boolean up;

	////////////////////
	// Source methods //
	////////////////////

	public Object transportQuery( Object query ) throws Exception
	{
		return transport.transportQuery( query );
	}

	public void transportControl( Object control, Object value ) throws Exception
	{
		transport.transportControl( control, value );
	}

	public void transportNotify( Object event ) throws Exception
	{
		transport.transportNotify( event );
	}

	public SessionMessage getSession()
	{
		return session;
	}
	
	public void setSession( SessionMessage session )
	{
		this.session = session;
	}
	
	private SessionMessage session;
}
