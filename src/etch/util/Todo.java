/* $Id: Todo.java 21902 2006-04-12 13:28:38Z wert $
 *
 * Created by wert on Mar 14, 2005.
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util;

/**
 * A Todo is used to perform a lightweight action.
 * 
 * @see TodoManager
 */
public interface Todo
{
	/**
	 * Performs the action.
	 * @param mgr the todo manager where this todo was queued. 
	 * @throws Exception if there is a problem.
	 */
	public void doit( TodoManager mgr ) throws Exception;

	/**
	 * Reports an exception that occurred while running the todo.
	 * @param mgr the todo manager where this todo was queued.
	 * @param e the exception that the todo threw.
	 */
	public void exception( TodoManager mgr, Exception e );
}
