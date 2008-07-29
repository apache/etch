// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package metreos.core.io;

import java.io.IOException;

import metreos.core.Who;
import metreos.util.FlexBuffer;
import metreos.util.URL;
import etch.bindings.java.transport.Resources;

/**
 * Packetizes a stream data source. Reads a packet header:
 * a 32-bit flag and a 32-bit length, bot big-endian, verifies
 * the flag, and then, using the length from the header,
 * reads the packet data and passes it to the packet handler.
 * As a packet source, accepts a packet and prepends a packet
 * header to it before delivering it to a data source.
 */
public final class Packetizer
	implements DataHandler, PacketSource
{
	/**
	 * URI term to specify max packet size.
	 */
	public final static String MAX_PKT_SIZE_TERM = "Packetizer.maxPktSize";
	
	/**
	 * The default maximum packet size that will be accepted,
	 * 10240 bytes.
	 */
	public final static int DEFAULT_MAX_PKT_SIZE = 10240;

	/**
	 * Constructs the Packetizer with the specified packet handler and maximum
	 * packet size.
	 * 
	 * @param handler the handler to receive the packets.
	 * 
	 * @param maxPktSize the maximum packet size that will be accepted.
	 * Must be >= 0. If maxPktSize == 0, the default will be used.
	 */
	public Packetizer( PacketHandler handler, int maxPktSize )
	{
		if (maxPktSize < 0)
			throw new IllegalArgumentException( "maxPktSize <= 0" );
		
		if (maxPktSize == 0)
			maxPktSize = DEFAULT_MAX_PKT_SIZE;
		
		this.handler = handler;
		this.maxPktSize = maxPktSize;
	}
	
	/**
	 * Constructs the Packetizer with the specified packet handler and default
	 * maximum packet size.
	 *
	 * @param handler the handler to receive the packets.
	 * 
	 * @see #DEFAULT_MAX_PKT_SIZE
	 */
	public Packetizer( PacketHandler handler )
	{
		this( handler, DEFAULT_MAX_PKT_SIZE );
	}
	
	/**
	 * Constructs the Packetizer with null packet handler and the specfied
	 * maximum packet size.
	 *
	 * @param maxPktSize the maximum packet size that will be accepted.
	 * Must be > 0.
	 * 
	 * @see #setHandler(PacketHandler)
	 */
	public Packetizer( int maxPktSize )
	{
		this( null, maxPktSize );
	}
	
	/**
	 * Constructs the Packetizer with null packet handler and default maximum
	 * packet size.
	 * 
	 * @see #setHandler(PacketHandler)
	 * @see #DEFAULT_MAX_PKT_SIZE
	 */
	public Packetizer()
	{
		this( null, DEFAULT_MAX_PKT_SIZE );
	}
	
	/**
	 * Constructs the Packetizer with null packet handler and uri specified
	 * or defaulted maximum packet size.
	 * 
	 * @param uri
	 * @param resources 
	 * 
	 * @see #setHandler(PacketHandler)
	 * @see #MAX_PKT_SIZE_TERM
	 * @see #DEFAULT_MAX_PKT_SIZE
	 */
	public Packetizer( URL uri, Resources resources )
	{
		this( null, uri.getIntegerTerm( MAX_PKT_SIZE_TERM, DEFAULT_MAX_PKT_SIZE ) );
	}
	
	private final int maxPktSize;
	
	@Override
	public String toString()
	{
		return String.format( "Packetizer/%s", src );
	}

	public void data( Who sender, FlexBuffer buf ) throws Exception
	{
		// there are two options here. one is that we have no buffered data
		// and the entire packet is contained within the buf. in that case
		// we could optimize the daylights out of the process and directly
		// drop the packet on the handler.
		
		while (buf.avail() > 0)
		{
			if (wantHeader)
			{
				// do we have enough to make a header?
				
				if (savedBuf.length() + buf.avail() >= HEADER_SIZE)
				{
					int len;
					
					if (savedBuf.length() == 0)
					{
						// savedBuf is empty, entire header in buf.
						
						len = processHeader( buf, false );
					}
					else // header split across savedBuf and buf
					{
						// move just enough data from buf to savedBuf to have a header.
						
						int needFromBuf = HEADER_SIZE - savedBuf.length();
						savedBuf.put( buf, needFromBuf );
						savedBuf.setIndex( 0 );
						
						len = processHeader( savedBuf, true );
					}
					
					if (len == 0)
						continue;
					
					bodyLen = len;
					wantHeader = false;
				}
				else // want header, but there's not enough to make it.
				{
					// save buf in savedBuf.
					
					savedBuf.setIndex( savedBuf.length() );
					savedBuf.put( buf );
				}
			}
			else if (savedBuf.length() + buf.avail() >= bodyLen)
			{
				// want body, and there's enough to make it.
				
				// three possible cases: the body is entirely in savedBuf,
				// the body is split, or the body is entirely in buf. assert
				// that the body cannot entirely be in savedBuf, or else
				// we'd have processed it last time.
				assert savedBuf.length() < bodyLen;
				
				if (savedBuf.length() == 0)
				{
					// savedBuf is empty, entire body in buf.

					int length = buf.length();
					int index = buf.index();
					buf.setLength( index+bodyLen );
					
					handler.packet( sender, buf );
					
					buf.setLength( length );
					buf.setIndex( index+bodyLen );
					
					wantHeader = true;
				}
				else // body split across savedBuf and buf
				{
					// move just enough data from buf to savedBuf to have a body.
					
					int needFromBuf = bodyLen - savedBuf.length();
					savedBuf.put( buf, needFromBuf );
					savedBuf.setIndex( 0 );
					
					handler.packet( sender, savedBuf );
					
					savedBuf.reset();
					wantHeader = true;
				}
			}
			else // want body, but there's not enough to make it.
			{
				// save buf in savedBuf.
				
				savedBuf.put( buf );
			}
		}
		// buf is now empty, and there's nothing else to do.
		assert buf.avail() == 0;
	}
	
	private int processHeader( FlexBuffer buf, boolean reset ) throws IOException
    {
		int sig = buf.getInt();
		if (sig != SIG)
			throw new IOException( "bad SIG" );
		
		int len = buf.getInt();
		
		if (reset)
			buf.reset();
		
		if (len < 0 || len > maxPktSize)
			throw new IOException( "len < 0 || len > maxPktSize" );
		
		return len;
    }

	private boolean wantHeader = true;
	
	private int bodyLen;
	
	private final FlexBuffer savedBuf = new FlexBuffer();

	public void packet( Who recipient, FlexBuffer buf ) throws Exception
	{
		// dataize the packet.
		
		// assert index is at the start of the header.
		int len = buf.avail();
		if (len < HEADER_SIZE)
			throw new IllegalArgumentException( "len < HEADER_SIZE" );
		
		int index = buf.index();
		buf.putInt( SIG );
		buf.putInt( len - HEADER_SIZE );
		buf.setIndex( index );
		src.data( recipient, buf );
	}

	public int headerSize()
	{
		return HEADER_SIZE;
	}
	
	private final static int SIG = 0xdeadbeef;
	
	private final static int HEADER_SIZE = 8;

	///////////////////////////
	// SourceHandler methods //
	///////////////////////////
	
	public DataSource getSource()
	{
		return src;
	}
	
	public void setSource( DataSource src )
	{
		this.src = src;
	}
	
	private DataSource src;

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

	public PacketHandler getHandler()
    {
	    return handler;
    }
	
	public void setHandler( PacketHandler handler )
	{
		this.handler = handler;
		handler.setSource( this );
	}

	private PacketHandler handler;

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

