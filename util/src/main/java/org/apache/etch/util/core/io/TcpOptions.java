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

import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;

/**
 * TCP connection options.
 */
public class TcpOptions
{
	/**
	 * Constructs TcpOptions from uri and resources.
	 * 
	 * @param uri
	 * @param resources
	 */
	public TcpOptions( URL uri, Resources resources )
	{
		autoFlush = uri.getBooleanTerm( AUTO_FLUSH, false );
		bufferSize = checkBufferSize( uri.getIntegerTerm( BUFFER_SIZE, 0 ) );
		keepAlive = uri.getBooleanTerm( KEEP_ALIVE, false );
		lingerTime = checkLingerTime( uri.getIntegerTerm( LINGER_TIME, 30 ) );
		noDelay = uri.getBooleanTerm( NO_DELAY, true );
		reconnectDelay = checkReconnectDelay( uri.getIntegerTerm(
			RECONNECT_DELAY, 0 ) );
		trafficClass = checkTrafficClass( uri.getIntegerTerm( TRAFFIC_CLASS, 0 ) );
	}

	/**
	 * Term on the uri which specifies the auto flush flag. The term string is
	 * "TcpTransport.autoFlush". The value is "true" or "false". The default is
	 * "false". Auto-flush only applies when the stream is buffered. True means
	 * that any "sent" data is automatically pushed to the operating systems
	 * without buffering.
	 */
	public final static String AUTO_FLUSH = "TcpTransport.autoFlush";

	/**
	 * Term on the uri which specifies the output buffer size in bytes. The term
	 * string is "TcpTransport.bufferSize". The value is an integer between 0
	 * and 65536. The default is 0, which means no output buffering.
	 */
	public final static String BUFFER_SIZE = "TcpTransport.bufferSize";

	/**
	 * Term on the uri which specifies the keep alive flag. The term string is
	 * "TcpTransport.keepAlive". The value is "true" or "false". The default is
	 * "false". Here keep alive refers to TCP specified keep alive, which is not
	 * the same as any application level keep alive.
	 */
	public final static String KEEP_ALIVE = "TcpTransport.keepAlive";

	/**
	 * Term on the uri which specifies the linger on close time in seconds. The
	 * term string is "TcpTransport.lingerTime". The value is an integer between
	 * -1 and 65535. The default is 30. The value -1 means "no linger on close".
	 * The value determines how long close will wait for buffered but
	 * unacknowledged data to be delivered. When the time expires, the
	 * connection will be forcefully closed. (The difference between a linger of
	 * -1 and a linger of 0 is subtle, but comes down to a close with a linger
	 * of -1 means perform forceful close while a close with a linger of 0 means
	 * perform a graceful close if there is no buffered data and a forceful
	 * close if there is buffered data. A forceful close is send RST, while a
	 * graceful close means send FIN and wait for FINACK.)
	 */
	public final static String LINGER_TIME = "TcpTransport.lingerTime";

	/**
	 * Term on the uri which specifies the no delay flag. The term string is
	 * "TcpTransport.noDelay". The value is "true" or "false". The default is
	 * "true". When true, the operating system will make a best effort to
	 * transmit data ASAP. When false, data might be delayed somewhat in order
	 * to allow for more efficient transmission by combining packets (see
	 * Nagle's Algorithm).
	 */
	public final static String NO_DELAY = "TcpTransport.noDelay";

	/**
	 * Term on the uri which specifies the reconnect delay in milliseconds. The
	 * term string is "TcpTransport.reconnectDelay". The value is an integer >=
	 * 0. The default is 0. The value 0 means no automatic reconnection is
	 * desired.
	 */
	public final static String RECONNECT_DELAY = "TcpTransport.reconnectDelay";

	/**
	 * Term on the uri which specifies the traffic class. The term string is
	 * "TcpTransport.trafficClass". The value is an integer between 0 and 255.
	 * The default is 0. The value of this field is network and service specific
	 * value. It is mapped to the IPv4 type of service (TOS) field and the IPv6
	 * traffic class field. See also Differentiated Services (DSCP).
	 */
	public final static String TRAFFIC_CLASS = "TcpTransport.trafficClass";

	/**
	 * The auto flush setting for this connection. If true, each call to send
	 * must automatically call flush.
	 */
	public final boolean autoFlush;

	private static int checkBufferSize( int bufferSize )
	{
		if (bufferSize < 0 || bufferSize > 65536)
			throw new IllegalArgumentException(
				"bufferSize < 0 || bufferSize > 65536" );
		return bufferSize;
	}

	/**
	 * The output buffer size to use for this connection. The value is specified
	 * as bytes, 0 means unbuffered output. If using buffered output, you'll
	 * want to disable auto flush and call flush manually only when needed. Only
	 * meaningful for stream protocols and implementations which use a stream
	 * interface to the operating system.
	 */
	public final int bufferSize;

	/** The tcp keep alive setting for this connection. */
	public final boolean keepAlive;

	private static int checkLingerTime( int lingerTime )
	{
		if (lingerTime < -1 || lingerTime > 240)
			throw new IllegalArgumentException(
				"lingerTime < -1 || lingerTime > 240" );
		return lingerTime;
	}

	/**
	 * The tcp linger time setting for this connection. Time in seconds, -1
	 * means disable.
	 */
	public final int lingerTime;

	/**
	 * The tcp no delay setting for this connection. True disables nagle's
	 * algorithm and causes all sends to be made asap.
	 */
	public final boolean noDelay;

	private static int checkReconnectDelay( int reconnectDelay )
	{
		if (reconnectDelay < 0)
			throw new IllegalArgumentException( "reconnectDelay < 0" );
		return reconnectDelay;
	}

	/**
	 * The reconnect delay for this connection. Time in milliseconds, 0 means do
	 * not reconnect.
	 */
	public final int reconnectDelay;

	private static int checkTrafficClass( int trafficClass )
	{
		if (trafficClass < 0 || trafficClass > 255)
			throw new IllegalArgumentException(
				"trafficClass < 0 || trafficClass > 255" );
		return trafficClass;
	}

	/**
	 * The traffic class for this connection. 0-255, 0 means normal handling.
	 * Also called type of service or dscp.
	 */
	public final int trafficClass;
}
