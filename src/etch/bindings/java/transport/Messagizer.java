// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package etch.bindings.java.transport;


import metreos.core.Who;
import metreos.core.io.PacketHandler;
import metreos.core.io.PacketSource;
import metreos.util.FlexBuffer;
import metreos.util.URL;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.TaggedDataInput;
import etch.bindings.java.msg.TaggedDataOutput;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.transport.fmt.binary.BinaryTaggedDataInput;
import etch.bindings.java.transport.fmt.binary.BinaryTaggedDataOutput;

/**
 * Messagizer is a PacketHandler which accepts packets from
 * a PacketSource and turns them into messages for delivery
 * to a MessageHandler. Messagizer is also a MessageSource,
 * accepting messages and turning them into packets for delivery
 * to a PacketSource.
 */
public final class Messagizer
	implements PacketHandler, MessagizerMessageSource
{
	/**
	 * Name of format name in uri or resources. The value is "Messagizer.format".
	 * The result of looking up this name should be a String.
	 * 
	 * @see #Messagizer(URL, Resources)
	 */
	public static final String FORMAT = "Messagizer.format";
	
	/**
	 * Name of value factory in resources. The value is "Messagizer.valueFactor".
	 * The result of looking up this name should be a {@link ValueFactory} for
	 * the service.
	 * 
	 * @see #Messagizer(URL, Resources)
	 */
	public static final String VALUE_FACTORY = "Messagizer.valueFactory";

	/**
	 * Constructs the Messagizer with binary tagged data format.
	 *
	 * @param handler
	 * @param vf
	 */
	public Messagizer( MessageHandler<MessagizerMessageSource> handler, ValueFactory vf )
	{
		this.handler = handler;
		tdi = new BinaryTaggedDataInput( vf, (FlexBuffer) null );
		tdo = new BinaryTaggedDataOutput( null, vf );
	}
	
	/**
	 * Constructs the Messagizer with null handler and tagged data format
	 * constructed from the format factory.
	 * @param uri the uri being used to configure the transport stack.
	 * @param resources the associated set of resources for this service.
	 */
	public Messagizer( URL uri, Resources resources )
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
		
		tdi = ff.newTaggedDataInput( vf, uri );
		tdo = ff.newTaggedDataOutput( vf, uri );
	}
	
	private final TaggedDataInput tdi;
	
	private final TaggedDataOutput tdo;
	
	@Override
	public String toString()
	{
		return String.format( "Messagizer/%s", src );
	}

    public void packet( Who sender, FlexBuffer buf ) throws Exception
	{
		// messagize the packet.
		
		tdi.setBuffer( buf );
		Message msg = tdi.readMessage();
//		Log.report( "Messagizer.packet", "recv", msg );
		handler.message( sender, msg );
	}

	////////////////////
	// Message Source //
	////////////////////
	
	public void messagex( Who recipient, Message msg ) throws Exception
	{
		// packetize the message.
		
		synchronized (msgBuf)
		{
			try
			{
//				Log.report( "Messagizer.packet", "send", msg );
				// assert that msgBuf is reset.
				// leave space for the packet header
				msgBuf.skip( src.headerSize(), true );
				tdo.setBuffer( msgBuf );
				msg.writeMessage( tdo );
				msgBuf.setIndex( 0 );
				src.packet( recipient, msgBuf );
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

	public PacketSource getSource()
	{
		return src;
	}
	
	public void setSource( PacketSource src )
	{
		this.src = src;
	}
	
	private PacketSource src;

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
		// TODO close all mailboxes for delivery when the connection goes down.
		handler.sessionNotify( event );
	}

	////////////////////
	// Source methods //
	////////////////////
	
	public MessageHandler<MessagizerMessageSource> getHandler()
	{
		return handler;
	}

	public void setHandler( MessageHandler<MessagizerMessageSource> handler )
	{
		this.handler = handler;
		handler.setSource( this );
	}
	
	private MessageHandler<MessagizerMessageSource> handler;

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
