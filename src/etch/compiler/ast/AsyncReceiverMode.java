/* $Id$
 *
 * Created by wert on Jul 9, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.compiler.ast;

/**
 * Flag that controls how the message receiver dispatches
 * and runs the implementation method.
 */
public enum AsyncReceiverMode
{
	/**
	 * Async receiver mode is disabled. The message runs synchronously.
	 */
	NONE,
	
	/**
	 * Async receiver queued mode. The message is queued to a thread pool.
	 */
	QUEUED,
	
	/**
	 * Async receiver queued mode. The message is queued to a thread pool.
	 * This is an alias for QUEUED.
	 * @deprecated use queued instead.
	 */
	@Deprecated
	POOL,
	
	/**
	 * Async receiver free mode. THe message runs on a free thread.
	 */
	FREE
}
