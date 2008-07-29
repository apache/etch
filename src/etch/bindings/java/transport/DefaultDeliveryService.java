/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport;



import metreos.core.io.Connection;
import metreos.core.io.DataHandler;
import metreos.core.io.Packetizer;
import metreos.core.io.TcpConnection;
import metreos.util.Startable;
import metreos.util.URL;
import etch.bindings.java.msg.ValueFactory;

/**
 * Description of MyDeliveryService.
 */
public class DefaultDeliveryService
	extends AbstractDeliveryService
	implements Startable, MessageHandler<MailboxMessageSource>
{
	/**
	 * Constructs the MyDeliveryService.
	 * @param conn 
	 */
	public DefaultDeliveryService( Connection<DataHandler> conn )
	{
		this.conn = conn;
	}
	
	/**
	 * @param uri
	 * @param resources
	 */
	public DefaultDeliveryService( URL uri, Resources resources )
	{
		this.conn = null;
	}

	private final Connection<DataHandler> conn;

	/**
	 * @param host
	 * @param port
	 * @param delay
	 * @param mh
	 * @param vf
	 * @return the delivery service.
	 */
	public static AbstractDeliveryService tcpConnection( String host, int port,
		int delay, MessageHandler<MailboxMessageSource> mh, ValueFactory vf )
	{
		TcpConnection c = new TcpConnection( null, host, port, delay );
		
		Packetizer p = new Packetizer( null, Packetizer.DEFAULT_MAX_PKT_SIZE );
//		p.setSource( c );
		c.setHandler( p );

		Messagizer m = new Messagizer( null, vf );
//		m.setSource( p );
		p.setHandler( m );

		MailboxManager r = new MailboxManager( null, 0 );
//		r.setSource( m );
		m.setHandler( r );
		
		DefaultDeliveryService d = new DefaultDeliveryService( c );
//		d.setSource( r );
		r.setHandler( d );
		
		return d;
	}

	public boolean isStarted()
	{
		return conn.isStarted();
	}

	public void start() throws Exception
	{
		conn.start();
	}

	public void stop() throws Exception
	{
		conn.stop();
	}
	
	/**
	 * @param maxDelay
	 * @throws InterruptedException
	 */
	public void waitUp( int maxDelay ) throws InterruptedException
	{
		conn.waitUp( maxDelay );
	}

	/**
	 * @param maxDelay
	 * @throws InterruptedException
	 */
	public void waitDown( int maxDelay ) throws InterruptedException
	{
		conn.waitDown( maxDelay );
	}

	@Override
	public String toString()
	{
		// TODO Auto-generated method stub
		return null;
	}

//	public void down( MailboxMessageSource src ) throws Exception
//	{
//		stub.down( src );
//	}

//	public void up( MailboxMessageSource src ) throws Exception
//	{
//		stub.up( src );
//	}
}
