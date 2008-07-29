/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package metreos.core.io;

import java.net.Socket;

/**
 * Interface used to report listener events.
 */
public interface ListenerHandler extends SourceHandler<Listener>
{
	/**
	 * Reports that the listener has accepted a connection.
	 * @param s the socket of the connection.
	 * @throws Exception 
	 */
	public void accepted( Socket s ) throws Exception;
}
