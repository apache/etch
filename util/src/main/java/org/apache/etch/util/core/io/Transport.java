/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.util.core.io;

/**
 * Out-of-band query, control, and notification interface for transports.
 * @param <S> the subclass of Session for this Transport.
 */
public interface Transport<S extends Session>
{
	/**
	 * Name of value factory in resources.
	 */
	public static final String VALUE_FACTORY = "Transport.valueFactory";

	/**
	 * Returns the session for this transport.
	 * @return the session for this transport.
	 */
	public S getSession();
	
	/**
	 * Sets the session for this transport.
	 * @param session session to receive data from the transport.
	 */
	public void setSession( S session );
		
	/**
	 * Gets a configuration or operational value from the transport. The
	 * request is passed down the chain of transports until some transport
	 * recognizes the query, whereupon it returns the requested value.
	 * 
	 * @param query an object representing a query, which could be as
	 * simple as a string, integer, or enum, or more complex such as
	 * a class with instance variables for query terms.
	 * 
	 * @return the requested value, or null if not defined.
	 * 
	 * @throws UnsupportedOperationException if the query is not recognized
	 * by any transport (which is to say, if the last transport in the transport
	 * chain does not recognize it, it should throw this exception). Typically
	 * this would be some sort of transport mechanism such as tcp, udp, jms,
	 * http, sip, etc.
	 * 
	 * @throws Exception
	 */
	public Object transportQuery( Object query ) throws Exception;
	
	/**
	 * Sets a configuration or operational value in the transport. The
	 * request is passed down the chain of transports until some transport
	 * recognizes the control, whereupon it stores the specified value
	 * and returns.
	 * 
	 * @param control an object representing a control, which could be as
	 * simple as a string, integer, or enum, or more complex such as
	 * a class with instance variables for control terms.
	 * 
	 * @param value the value to set.
	 * 
	 * @throws IllegalArgumentException if the value is not the right
	 * type or if the value is inappropriate.
	 * 
	 * @throws UnsupportedOperationException if the control is not recognized
	 * by any transport (which is to say, if the last transport in the transport
	 * chain does not recognize it, it should throw this exception). Typically
	 * this would be some sort of transport mechanism such as tcp, udp, jms,
	 * http, sip, etc.
	 * 
	 * @throws Exception
	 */
	public void transportControl( Object control, Object value ) throws Exception;
	
	/**
	 * Notifies the chain of transports of the specified event. Unlike query
	 * and control operations above, events are always passed down to the
	 * bottom to allow all transports to notice them (though it is possible
	 * that an event might be delayed for a bit).
	 * 
	 * @param event a class which represents the event, possibly with
	 * parameters. The simplest event could be a string, integer, or enum,
	 * but any class instance will do (as long as some transport in the
	 * chain expects it).
	 * 
	 * @throws Exception
	 */
	public void transportNotify( Object event ) throws Exception;

	///////////////////////////////////////////////
	// Well-known queries, controls, and events. //
	///////////////////////////////////////////////
	
	/**
	 * Transport control which starts the transport stack. The argument
	 * should be null.
	 */
	public final static String START = "START";
	
	/**
	 * Transport control which starts the transport stack and waits
	 * for it to come up. The argument should be the integer number of
	 * milliseconds to wait before giving up.
	 */
	public final static String START_AND_WAIT_UP = "START_AND_WAIT_UP";
	
	/**
	 * Transport query which waits for the transport stack to come up.
	 */
	public class WaitUp
	{
		/**
		 * @param maxDelay max delay in milliseconds.
		 */
		public WaitUp( int maxDelay )
		{
			this.maxDelay = maxDelay;
		}
		
		/**
		 * max delay in milliseconds.
		 */
		public final int maxDelay;
	}
	
	/**
	 * Transport control which stops the transport stack. The argument
	 * should be null, or a Boolean reset (true for instant close,
	 * false for a nicer, gentler close).
	 */
	public final static String STOP = "STOP";

	/**
	 * Transport control which stops the transport stack and waits
	 * for it to go down. The argument should be the integer number of
	 * milliseconds to wait before giving up.
	 */
	public final static String STOP_AND_WAIT_DOWN = "STOP_AND_WAIT_DOWN";

	/**
	 * Transport control which resets the transport stack (e.g., closes the
	 * socket) without stopping it. If it is set to reconnect, then it will do
	 * that. Only meaningful for connection oriented transports. Others types
	 * will ignore this.
	 */
	public static final String RESET = "RESET";

	/**
	 * Transport query which asks is this a listener initiated connection or
	 * is this a client initiated connection.
	 */
	public static final String IS_SERVER = "IS_SERVER";

	/**
	 * Transport query which waits for the transport stack to go down.
	 */
	public class WaitDown
	{
		/**
		 * @param maxDelay max delay in milliseconds.
		 */
		public WaitDown( int maxDelay )
		{
			this.maxDelay = maxDelay;
		}

		/**
		 * max delay in milliseconds.
		 */
		public final int maxDelay;
	}
}
