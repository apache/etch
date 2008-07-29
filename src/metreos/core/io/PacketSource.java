// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package metreos.core.io;

import metreos.core.Who;
import metreos.util.FlexBuffer;

/**
 * Interface used to send binary packet data back to the source. The source
 * might have been a datagram connection, or a stream connection which has
 * been packetized. Some packet sources require a packet header. To avoid
 * having to copy the data, the packet source should be queried for the
 * size of the packet header, and that much space left at the start of
 * the packet.
 * 
 * Some packet sources might allow the selection of a recipient other
 * than the default. It is beyond the scope of this interface to specify
 * the exact meaning or format. Generally, pass the sender you receieved
 * at the packet handler interface as the recipient. Passing null generally
 * means use the default recipient, which depends upon the transport.
 * 
 * To prepare a FlexBuffer to send a packet:
 * 
 * PacketSource src = ... e.g., from PacketHandler.packet ...
 * Who recipient = ... e.g., from PacketHandler.packet ...
 * FlexBuffer buf = new FlexBuffer();
 * buf.skip( src.headerSize(), true );
 * buf.put( ... packet data ... );
 * buf.setIndex( 0 );
 * src.packet( recipient, buf );
 */
public interface PacketSource extends Source<PacketHandler>
{
	/**
	 * @return the number of bytes to leave at the front of the packet
	 * for a packet header. Might be 0 for datagram sources.
	 */
	public int headerSize();
	
	/**
	 * Sends a packet to the packet source.
	 * @param recipient the recipient as passed to the packet handler,
	 * or null to select the default recipient for this transport.
	 * @param buf the buffer with the packet to send. The packet header
	 * starts at index. The "available" bytes are sent.
	 * @throws Exception
	 */
	public void packet( Who recipient, FlexBuffer buf ) throws Exception;
}
