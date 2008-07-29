// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package metreos.core.io;

import metreos.core.Who;
import metreos.util.FlexBuffer;

/**
 * Interface used to deliver data from a stream source.
 */
public interface DataHandler extends SourceHandler<DataSource>
{
	/**
	 * Delivers data from a stream source.
	 * @param sender
	 * @param buf
	 * @throws Exception
	 */
	public void data( Who sender, FlexBuffer buf ) throws Exception;
}
