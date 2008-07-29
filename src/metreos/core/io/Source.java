// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package metreos.core.io;

import etch.bindings.java.transport.Transport;

/**
 * Common interface to sources of data, packets, or messages. Models
 * the various handler's view of a communication channel.
 * @param <H> The handler type of this source.
 */
public interface Source<H extends SourceHandler> extends Transport
{
	/**
	 * Gets the handler of this source.
	 * @return the handler.
	 */
	public H getHandler();
	
	/**
	 * Sets the handler of this source.
	 * @param handler 
	 */
	public void setHandler( H handler );
}
