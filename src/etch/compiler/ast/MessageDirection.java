/* $Id$
 *
 * Created by sccomer on March 28, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

/**
 * The message direction is whether the message is from client to
 * server or server to client or both.
 */
public enum MessageDirection
{
	/**
	 * This message is used by the client to invoke the server.
	 */
	SERVER,
	/**
	 * This message is used by the server to invoke the client.
	 */
	CLIENT,
	/**
	 * This message may be used by either the client or server to invoke the other.
	 */
	BOTH
}

