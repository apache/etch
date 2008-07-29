// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package metreos.core.io;

import metreos.core.Who;
import metreos.util.FlexBuffer;

/**
 * Interface used to deliver packets from a packet source.
 */
public interface PacketHandler extends SourceHandler<PacketSource>
{
	/**
	 * Delivers a packet from a packet source.
	 * @param sender
	 * @param buf
	 * @throws Exception
	 */
	public void packet( Who sender, FlexBuffer buf ) throws Exception;
}
