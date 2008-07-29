/* $Id$
 *
 * Created by sccomer on Mar 29, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.support;

import metreos.core.Who;
import etch.bindings.java.msg.Message;

/**
 * Interface which a service implementation may implement which
 * is used by the service stub to give notice of an unwanted
 * message. This is a message which was not wanted by the service
 * stubs (its id did not match any service method).
 */
public interface Unwanted
{
	/**
	 * Notifies the service implementation that the message
	 * is unwanted, that is, its id does not match any defined message.
	 * @param sender the transport defined sender.
	 * @param msg the message that was not wanted.
	 */
	public void _unwanted( Who sender, Message msg );
}
