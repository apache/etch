/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport;


import metreos.core.io.DataSource;
import metreos.core.io.Packetizer;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.StubBase;

/**
 * Another variation on DeliveryService.
 * @param <T> the type of the stub.
 */
abstract public class DefaultDeliveryService2<T extends StubBase>
	extends AbstractDeliveryService
	implements MessageHandler<MailboxMessageSource> 

{
	/**
	 * Constructs the DefaultDeliveryService2.
	 */
	public DefaultDeliveryService2()
	{
		// Nothing to do.
	}
	
	/**
	 * @param src
	 * @return new stub
	 */
	abstract protected T newStub( MailboxMessageSource src );
	
//	public void down(MailboxMessageSource src) throws Exception 
//	{
//		
//		Log.report( "messageHandlerDown", "who", this, "src", src );
//		if (stub != null)
//		{
//			stub.message( null, null );
//			stub = null;
//		}
//		
//	}

//	public void up(MailboxMessageSource src) throws Exception 
//	{
//		
//		Log.report( "messageHandlerUp", "who", this, "src", src );
//		stub = newStub( src );
//		
//	}
	
	/**
	 * @param <T>
	 * @param dds
	 * @param c
	 * @param vf
	 * @return an instance of delivery service to use as a message handler.
	 */
	public static <T extends StubBase> DefaultDeliveryService2<T>
		setUp( DefaultDeliveryService2<T> dds, DataSource c,  ValueFactory vf)
	{
		MailboxManager mgr = new MailboxManager( dds, 0 );
		
		Messagizer m = new Messagizer( mgr, vf );
		mgr.setSource( m );
		
		Packetizer p = new Packetizer( m, Packetizer.DEFAULT_MAX_PKT_SIZE );
		m.setSource( p );
		p.setSource( c );
		
		dds.setSource(mgr);
		c.setHandler(p);
		
		return dds;
	}
	
	@Override
	public void sessionNotify( Object event )
	{
		if (event.equals( Session.UP ))
		{
			addStub( newStub( getSource() ) );
		}
		
		super.sessionNotify( event );
		
		if (event.equals( Session.DOWN ))
		{
			removeStub();
		}
	}
}
