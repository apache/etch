// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package etch.bindings.java.transport;

import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.ValueFactory;
import etch.util.FlexBuffer;
import etch.util.Resources;
import etch.util.URL;
import etch.util.core.Who;
import etch.util.core.io.SessionPacket;
import etch.util.core.io.TransportPacket;

/**
 * Messagizer is a PacketHandler which accepts packets from
 * a PacketSource and turns them into messages for delivery
 * to a MessageHandler. Messagizer is also a MessageSource,
 * accepting messages and turning them into packets for delivery
 * to a PacketSource.
 */
public final class Messagizer implements SessionPacket, TransportMessage
{
	/**
	 * Name of format name in uri or resources. The value is "Messagizer.format".
	 * The result of looking up this name should be a String.
	 * 
	 * @see #Messagizer(TransportPacket, String, Resources)
	 */
	public static final String FORMAT = "Messagizer.format";
	
	/**
	 * Name of value factory in resources. The value is "Messagizer.valueFactor".
	 * The result of looking up this name should be a {@link ValueFactory} for
	 * the service.
	 * 
	 * @see #Messagizer(TransportPacket, String, Resources)
	 */
	public static final String VALUE_FACTORY = "Messagizer.valueFactory";
	
	/**
	 * Constructs the Messagizer with null handler and tagged data format
	 * constructed from the format factory.
	 * @param transport 
	 * @param uri the uri being used to configure the transport stack.
	 * @param resources the associated set of resources for this service.
	 */
	public Messagizer( TransportPacket transport, String uri, Resources resources )
	{
		this( transport, new URL( uri ), resources );
	}
	
	/**
	 * Constructs the Messagizer with null handler and tagged data format
	 * constructed from the format factory.
	 * @param uri the uri being used to configure the transport stack.
	 * @param resources the associated set of resources for this service.
	 */
	private Messagizer( TransportPacket transport, URL uri, Resources resources )
	{
		// find the format.
		
		String format = uri.getTerm( FORMAT );
		if (format == null)
		{
			format = (String) resources.get( FORMAT );
			if (format == null)
				throw new IllegalArgumentException(
					String.format( "format name is not defined as '%s' in uri or resources", FORMAT ) );
		}
		
		// find the format factory.
		
		FormatFactory ff = FormatFactory.get( format );
		if (ff == null)
			throw new IllegalArgumentException(
				String.format( "format factory is not defined as '%s' in format factories", format ) );
		
		// find the value factory.
		
		ValueFactory vf = (ValueFactory) resources.get( VALUE_FACTORY );
		if (vf == null)
			throw new IllegalArgumentException(
				String.format( "value factory is not defined as '%s' in resources", VALUE_FACTORY ) );
		
		// construct the message formatters.
		
		this.transport = transport;
		tdi = ff.newTaggedDataInput( vf, uri.toString() );
		tdo = ff.newTaggedDataOutput( vf, uri.toString() );
		
		transport.setSession( this );
	}
	
	private final TransportPacket transport;
	
	private final TaggedDataInput tdi;
	
	private final TaggedDataOutput tdo;
	
	/**
	 * @return the transport.
	 */
	public TransportPacket getTransport()
	{
		return transport;
	}
	
	@Override
	public String toString()
	{
		return String.format( "Messagizer/%s", transport );
	}

    public void sessionPacket( Who sender, FlexBuffer buf ) throws Exception
	{
		// messagize the packet.
		
		Message msg = tdi.readMessage( buf );
//		Log.report( "Messagizer.packet", "who", this, "recv", msg );
		boolean handled = session.sessionMessage( sender, msg );
		if (!handled)
			session.sessionNotify( new UnwantedMessage( sender, msg ) );
	}

	////////////////////
	// Message Source //
	////////////////////
	
	public void transportMessage( Who recipient, Message msg ) throws Exception
	{
		// packetize the message.
		
		synchronized (msgBuf)
		{
			try
			{
//				Log.report( "Messagizer.packet", "who", this, "send", msg );
				// assert that msgBuf is reset.
				// leave space for the packet header
				msgBuf.skip( transport.headerSize(), true );
				tdo.writeMessage( msg, msgBuf );
				msgBuf.setIndex( 0 );
				transport.transportPacket( recipient, msgBuf );
			}
			finally
			{
				msgBuf.reset();
			}
		}
	}
	
	private final FlexBuffer msgBuf = new FlexBuffer();
	
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
		session.sessionNotify( event );
	}

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
