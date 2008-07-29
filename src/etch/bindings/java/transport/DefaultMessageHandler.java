/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport;

import metreos.core.Who;
import etch.bindings.java.msg.Message;
import etch.bindings.java.support.StubBase;

/**
 * Description of DefaultMessageHandler.
 * @param <T> The type of the stub
 */
abstract public class DefaultMessageHandler<T extends StubBase>
	implements MessageHandler<MailboxMessageSource>
{
	/**
	 * Constructs the DefaultMessageHandler.
	 */
	public DefaultMessageHandler()
	{
		// nothing to do.
	}
	
	/**
	 * @param src
	 * @return new stub
	 */
	abstract protected T newStub( MailboxMessageSource src );
	
	@Override
	abstract public String toString();

	private T stub;

	public boolean message( Who sender, Message msg ) throws Exception
	{
		return stub.message( sender, msg );
	}
	
	public MailboxMessageSource getSource()
	{
		return src;
	}
	
	public void setSource( MailboxMessageSource src )
	{
		this.src = src;
	}
	
	private MailboxMessageSource src;
	
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
		if (event.equals( Session.UP ))
		{
			stub = newStub( src );
			return;
		}

		if (event.equals( Session.DOWN ))
		{
			stub = null;
			return;
		}
		
		// ignore.
	}
}