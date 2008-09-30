/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.util.core.io;

import etch.util.DataInput;
import etch.util.core.Who;

/**
 * Interface used to deliver packets to the session from the transport.
 */
public interface SessionPacket extends Session
{
	/**
	 * Delivers a packet to the session from the transport.
	 * @param sender the sender of the packet, for transports which allow
	 * multiple senders. This is who to return any response to.
	 * @param buf a FlexBuffer positioned at the packet.
	 * @throws Exception
	 */
	public void sessionPacket( Who sender, DataInput buf ) throws Exception;
}
