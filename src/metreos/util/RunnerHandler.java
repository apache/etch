/* $Id$
 *
 * Created by wert on Mar 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package metreos.util;

/**
 * RunnerHandler receives notification of runner events.
 */
public interface RunnerHandler
{
	/**
	 * Reports that the thread controlling the runner has started.
	 */
	void started();

	/**
	 * Reports that the thread controlling the runner has stopped.
	 */
	void stopped();

	/**
	 * Reports that the thread controlling the runner has caught an
	 * exception.
	 * @param what a short description of the origin
	 * @param e an exception that was caught
	 */
	void exception( String what, Exception e );
}
