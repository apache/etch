package etch.bindings.java.support;

/**
 * The asynchronous receiver mode of this helper.
 */
public enum AsyncMode
{
	/** synchronous mode */
	NONE,
	/** the operation is queued to a thread pool for execution */
	QUEUED,
	/** the operation is executed by a newly allocated thread */
	FREE
}