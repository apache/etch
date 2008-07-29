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

package etch.bindings.java.transport;

import etch.bindings.java.msg.Message;
import etch.util.core.Who;

/**
 * Event class used with sessionNotify to report unwanted messages.
 */
public class UnwantedMessage
{
	/**
	 * Constructs the UnwantedMessage.
	 * @param sender
	 * @param msg
	 */
	public UnwantedMessage( Who sender, Message msg )
	{
		this.sender = sender;
		this.msg = msg;
	}
	
	/** the sender of this unwanted message. */
	public final Who sender;
	
	/** the unwanted message. */
	public final Message msg;
	
	@Override
	public String toString()
	{
		return String.format( "Unwanted message from %s: %s", sender, msg );
	}
}
