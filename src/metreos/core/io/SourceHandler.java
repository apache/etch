// $Id$
//
// Created by wert on Apr 8, 2007.
//
// Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.

package metreos.core.io;

import etch.bindings.java.transport.Session;

/**
 * SourceHandler receives notification of source events.
 * @param <S> event originator
 */
public interface SourceHandler<S extends Source> extends Session
{
	/**
	 * @return the source.
	 */
	public S getSource();
	
	/**
	 * Sets the source.
	 * @param src
	 */
	public void setSource( S src );
}
