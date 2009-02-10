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

package org.apache.etch.util.core.io;

import java.io.IOException;

import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;


/**
 * Packetizes a stream data source. Reads a packet header:
 * a 32-bit flag and a 32-bit length, both big-endian, verifies
 * the flag, and then, using the length from the header,
 * reads the packet data and passes it to the packet handler.
 * As a packet source, accepts a packet and prepends a packet
 * header to it before delivering it to a data source.
 */
public final class Packetizer implements SessionData, TransportPacket
{
	/**
	 * URI term to specify max packet size.
	 */
	public final static String MAX_PKT_SIZE_TERM = "Packetizer.maxPktSize";
	
	private final static int SIG = 0xdeadbeef;
	
	private final static int HEADER_SIZE = 8;
	
	/**
	 * The default maximum packet size that will be accepted, 16376 bytes.
	 */
	public final static int DEFAULT_MAX_PKT_SIZE = 16384 - HEADER_SIZE;
	
	/**
	 * Constructs the Packetizer with null packet handler and uri specified
	 * or defaulted maximum packet size.
	 * @param transport used to deliver data to the peer.
	 * @param uri
	 * @param resources
	 */
	public Packetizer( TransportData transport, String uri, Resources resources )
	{
		this( transport, new URL( uri ), resources );
	}
	
	/**
	 * Constructs the Packetizer with null packet handler and uri specified
	 * or defaulted maximum packet size.
	 * 
	 * @param transport used to deliver data to the peer.
	 * @param uri
	 * @param resources
	 * 
	 * @see #MAX_PKT_SIZE_TERM
	 * @see #DEFAULT_MAX_PKT_SIZE
	 */
	public Packetizer( TransportData transport, URL uri, Resources resources )
	{
		this( transport, uri.getIntegerTerm( MAX_PKT_SIZE_TERM, DEFAULT_MAX_PKT_SIZE ) );
	}

	/**
	 * Constructs the Packetizer with the specified packet handler and maximum
	 * packet size.
	 * 
	 * @param handler the handler to receive the packets.
	 * 
	 * @param maxPktSize the maximum packet size that will be accepted.
	 * Must be >= 0. If maxPktSize == 0, there is no limit.
	 */
	private Packetizer( TransportData transport, int maxPktSize )
	{
		if (maxPktSize < 0)
			throw new IllegalArgumentException( "maxPktSize < 0" );
		
		this.transport = transport;
		this.maxPktSize = maxPktSize;
		
		transport.setSession( this );
	}
	
	private final TransportData transport;
	
	private final int maxPktSize;
	
	/**
	 * @return the transport.
	 */
	public TransportData getTransport()
	{
		return transport;
	}
	
	@Override
	public String toString()
	{
		return String.format( "Packetizer/%s", transport );
	}

	public void sessionData( Who sender, FlexBuffer buf ) throws Exception
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
					int pktSize;
					
					if (savedBuf.length() == 0)
					{
						// savedBuf is empty, entire header in buf.
						
						pktSize = processHeader( buf, false );
					}
					else // header split across savedBuf and buf
					{
						// move just enough data from buf to savedBuf to have a header.
						
						int needFromBuf = HEADER_SIZE - savedBuf.length();
						savedBuf.put( buf, needFromBuf );
						savedBuf.setIndex( 0 );
						
						pktSize = processHeader( savedBuf, true );
					}
					
					if (pktSize == 0)
						continue;
					
					bodyLen = pktSize;
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
					
					session.sessionPacket( sender, buf );
					
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
					
					session.sessionPacket( sender, savedBuf );
					
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
		
		int pktSize = buf.getInt();
		
		if (reset)
			buf.reset();
		
		if (pktSize < 0 || maxPktSize > 0 && pktSize > maxPktSize)
			throw new IOException( "pktSize < 0 || (maxPktSize > 0 && pktSize > maxPktSize)" );
		
		return pktSize;
    }

	private boolean wantHeader = true;
	
	private int bodyLen;
	
	private final FlexBuffer savedBuf = new FlexBuffer();

	public void transportPacket( Who recipient, FlexBuffer buf ) throws Exception
	{
		// data-ize the packet.
		
		// assert index is at the start of the header.
		int dataSize = buf.avail();
		if (dataSize < HEADER_SIZE)
			throw new IllegalArgumentException( "dataSize < HEADER_SIZE" );
		
		int pktSize = dataSize - HEADER_SIZE;
		if (maxPktSize > 0 && pktSize > maxPktSize)
			throw new IOException( "maxPktSize > 0 && pktSize > maxPktSize" );
		
		int index = buf.index();
		buf.putInt( SIG );
		buf.putInt( pktSize );
		buf.setIndex( index );
		transport.transportData( recipient, buf );
	}

	public int headerSize()
	{
		return HEADER_SIZE;
	}

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

	public SessionPacket getSession()
	{
		return session;
	}

	public void setSession( SessionPacket session )
	{
		this.session = session;
	}
	
	private SessionPacket session;
}

