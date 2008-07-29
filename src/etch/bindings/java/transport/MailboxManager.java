// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package etch.bindings.java.transport;


import java.util.HashMap;
import java.util.Map;

import metreos.core.Who;
import metreos.util.IdGenerator;
import metreos.util.Log;
import metreos.util.URL;
import etch.bindings.java.msg.Message;
import etch.bindings.java.support.Mailbox;

/**
 * MailboxManager is a MessageHandler which accepts packets for
 * possible delivery to a mailbox, or to another message handler
 * if an appropriate mailbox cannot be found. MailboxManager is
 * also a MessageSource, tagging messages with msgids and then
 * forwarding them to another MessageSource. If requested, a
 * mailbox is created with a message's msgid and added to the
 * set of mailboxes waiting for messages.
 */
public class MailboxManager
	implements MessageHandler<MessagizerMessageSource>, MailboxMessageSource
{
	/**
	 * @param defaultHandler a message handler to receive messages which
	 * cannot be delivered to a mailbox.
	 * @param maxDelay maximum amount of time to wait for a full mailbox
	 * to allow a new message before giving up and delivering to the
	 * message handler. 0 means wait forever, -1 means don't wait at
	 * all.
	 */
	public MailboxManager( MessageHandler<MailboxMessageSource> defaultHandler,
		int maxDelay )
	{
		this.handler = defaultHandler;
		this.maxDelay = maxDelay;
	}
	
	/**
	 * @param uri
	 * @param resources
	 */
	public MailboxManager( URL uri, Resources resources )
	{
		this( null, 0 );
	}
	
	private final int maxDelay;
	
	@Override
	public String toString()
	{
		return String.format( "MailboxManager/%s", src );
	}

	////////////////////////////
	// MessageHandler methods //
	////////////////////////////
	
	public boolean message( Who sender, Message msg ) throws Exception
	{
		Long msgid = msg.getInReplyTo();
		if (msgid != null)
		{
			Mailbox mb = getMailbox( msgid );
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
			// no such mailbox - fall through
		}
		// no such mailbox or no msgid - fall through
		
		boolean delivered = handler.message( sender, msg );
		
		if (!delivered)
			Log.report( "MailboxManager.undelivered", "msg", msg );
		
		return delivered;
	}

	///////////////////////////
	// MessageSource methods //
	///////////////////////////

	public void messagex( Who recipient, Message msg ) throws Exception
	{
		msg.setMessageId( idGen.next() );
		src.messagex( recipient, msg );
	}

	public Mailbox message( Who recipient, Message msg, int maxMessages,
		int lifetime ) throws Exception
	{
		if (lifetime < 0)
			throw new IllegalArgumentException( "lifetime < 0" );
		
		Long msgid = idGen.next();
		msg.setMessageId( msgid );
		
		Mailbox mb =
			new PlainMailbox(
				this, msgid, maxDelay, lifetime, maxMessages );
		register( mb );
		
		try
		{
			src.messagex( recipient, msg );
		}
		catch ( Exception e )
		{
			unregister( mb );
			throw e;
		}
		
		return mb;
	}
	
	private final IdGenerator idGen = new IdGenerator( System.currentTimeMillis(), 37 );

//	public void close( boolean reset ) throws Exception
//	{
//		msgSrc.close( reset );
//	}

//	public SocketAddress localAddress() throws Exception
//	{
//		return msgSrc.localAddress();
//	}

//	public SocketAddress remoteAddress() throws Exception
//	{
//		return msgSrc.remoteAddress();
//	}

//	public void shutdownInput() throws Exception
//	{
//		msgSrc.shutdownInput();
//	}

//	public void shutdownOutput() throws Exception
//	{
//		msgSrc.shutdownOutput();
//	}

//	public void stop() throws Exception
//	{
//		msgSrc.stop();
//	}

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
			if (mailboxes.containsKey( msgid ))
				throw new IllegalArgumentException( "dup msgid in mailboxes" );
			
			mailboxes.put( msgid, mb );
		}
	}

	/**
	 * Removes the mailbox from the set of mailbox receiving responses
	 * to messages.
	 * @param mb
	 */
	public void unregister( Mailbox mb )
	{
		synchronized (mailboxes)
		{
			mailboxes.remove( mb.getMessageId() );
		}
	}
	
	private Mailbox getMailbox( Long msgid )
	{
		synchronized (mailboxes)
		{
			return mailboxes.get( msgid );
		}
	}
	
	private Map<Long, Mailbox> mailboxes =
		new HashMap<Long, Mailbox>();

	/**
	 * @param sender 
	 * @param msg
	 * @throws Exception 
	 */
	public void redeliver( Who sender, Message msg ) throws Exception
	{
		handler.message( sender, msg );
	}

//	public void oobNotifySource( String event, Object... args )
//		throws Exception
//	{
//		msgSrc.oobNotifySource( event, args );
//	}
	
	///////////////////////////
	// SourceHandler methods //
	///////////////////////////

	public MessagizerMessageSource getSource()
	{
		return src;
	}
	
	public final void setSource( MessagizerMessageSource src )
	{
		this.src = src;
	}
	
	private MessagizerMessageSource src;

	public Object sessionQuery( Object query ) throws Exception
	{
		return handler.sessionQuery( query );
	}

	public void sessionControl( Object control, Object value ) throws Exception
	{
		handler.sessionControl( control, value );
	}

	public void sessionNotify( Object event ) throws Exception
	{
		handler.sessionNotify( event );
	}

	////////////////////
	// Source methods //
	////////////////////
	
	public MessageHandler<MailboxMessageSource> getHandler()
	{
		return handler;
	}
	
    public void setHandler( MessageHandler<MailboxMessageSource> handler )
	{
		this.handler = handler;
		handler.setSource( this );
	}

	private MessageHandler<MailboxMessageSource> handler;

	public Object transportQuery( Object query ) throws Exception
	{
		return src.transportQuery( query );
	}

	public void transportControl( Object control, Object value ) throws Exception
	{
		src.transportControl( control, value );
	}

	public void transportNotify( Object event ) throws Exception
	{
		src.transportNotify( event );
	}
}
