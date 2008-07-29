// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package etch.bindings.java.transport;

import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import etch.bindings.java.msg.Message;
import etch.bindings.java.support.Mailbox;
import etch.util.IdGenerator;
import etch.util.Resources;
import etch.util.core.Who;
import etch.util.core.io.Session;

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
	 * @param transport 
	 * @param maxDelay maximum amount of time to wait for a full mailbox
	 * to allow a new message before giving up and delivering to the
	 * message handler. 0 means wait forever, -1 means don't wait at
	 * all.
	 */
	private PlainMailboxManager( TransportMessage transport, int maxDelay )
	{
		this.transport = transport;
		this.maxDelay = maxDelay;
		
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
	
	/**
	 * @param transport 
	 * @param uri
	 * @param resources
	 */
	public PlainMailboxManager( TransportMessage transport, String uri,
		Resources resources )
	{
		this( transport, 0 );
	}
	
	private final int maxDelay;
	
	@Override
	public String toString()
	{
		return String.format( "MailboxManager/%s", transport );
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
		
		Mailbox mb = new PlainMailbox( this, msgid, maxDelay, 0, 1 );
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
			// TODO fix it so that this works. the waitUp construct needs to
			// wait from the top, not the bottom.
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
	
	@SuppressWarnings("unused")
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
