/* $Id$
 *
 * Created by wert on Apr 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport;

import metreos.core.io.Source;
import metreos.core.io.SourceHandler;

/**
 * Interface used to deliver messages from the messagizer.
 * @param <S> 
 */
public interface MessageHandler<S extends Source>
	extends SourceHandler<S>,
	etch.bindings.java.support.MessageHandler
{
	// nothing else.
}
