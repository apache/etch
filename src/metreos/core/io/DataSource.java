// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package metreos.core.io;

import metreos.core.Who;
import metreos.util.FlexBuffer;

/**
 * Interface used to model a stream source to a data recipient, which
 * allows the recipient to send data to the peer.
 */
public interface DataSource extends Source<DataHandler>
{
	/**
	 * Delivers data to the peer via the data stream.
	 * @param recipient
	 * @param buf
	 * @throws Exception
	 */
	public void data( Who recipient, FlexBuffer buf ) throws Exception;
}
