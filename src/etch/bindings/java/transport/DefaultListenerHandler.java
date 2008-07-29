/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport;

import java.net.Socket;

import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.core.io.Packetizer;
import metreos.core.io.TcpConnection;
import etch.bindings.java.msg.ValueFactory;

/**
 * Default listener handler. Accepts connections and constructs stubs
 * and value factory to handle it.
 */
abstract public class DefaultListenerHandler implements ListenerHandler
{
	public void accepted( Socket s ) throws Exception
	{
		ValueFactory vf = newValueFactory();
		
		MessageHandler<MailboxMessageSource> mh = newMessageHandler();
		
		MailboxManager mgr = new MailboxManager( mh, 0 );
		mh.setSource( mgr );
		
		Messagizer m = new Messagizer( mgr, vf );
		mgr.setSource( m );
		
		Packetizer p = new Packetizer( m, Packetizer.DEFAULT_MAX_PKT_SIZE );
		m.setSource( p );
		
		TcpConnection c = new TcpConnection( p, s );
		p.setSource( c );
		
		c.start();
	}

	/**
	 * @return an appropriate value factory for this connection.
	 */
	abstract protected ValueFactory newValueFactory();

	/**
	 * @return an appropriate message handler for this connection.
	 */
	abstract protected MessageHandler<MailboxMessageSource> newMessageHandler();
	
	@Override
	abstract public String toString();
	
	public Listener getSource()
	{
		return src;
	}
	
	final public void setSource( Listener src )
	{
		this.src = src;
	}
	
	private Listener src;
	
	public Object sessionQuery( Object query )
	{
		throw new UnsupportedOperationException( "unknown query: "+query );
	}
	
	public void sessionControl( Object control, Object value )
	{
		throw new UnsupportedOperationException( "unknown control: "+control );
	}
	
	public void sessionNotify( Object event )
	{
		// ignore.
	}
}