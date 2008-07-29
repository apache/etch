package etch.bindings.java.transport;

/**
 * Out-of-band query, control, and notification interface for sessions.
 */
public interface Session
{
	/**
	 * Gets a configuration or operational value from the handler. The
	 * request is passed up the chain of sessions until some session
	 * recognises the query, whereupon it returns the requested value.
	 * 
	 * @param query an object representing a query, which could be as
	 * simple as a string, integer, or enum, or more complex such as
	 * a class with instance variables for query terms.
	 * 
	 * @return the requested value, or null if not defined.
	 * 
	 * @throws UnsupportedOperationException if the query is not recognised
	 * by any session (which is to say, if the last session in the session
	 * chain does not recognise it, it should throw this exception). Typically
	 * this would be a service's client or server implementation.
	 * 
	 * @throws Exception 
	 */
	public Object sessionQuery( Object query ) throws Exception;
	
	/**
	 * Sets a configuration or operational value in the session. The
	 * request is passed up the chain of sessions until some session
	 * recognises the control, whereupon it stores the specified value
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
	 * @throws UnsupportedOperationException if the control is not recognised
	 * by any session (which is to say, if the last session in the session
	 * chain does not recognise it, it should throw this exception). Typically
	 * this would be a service's client or server implementation.
	 * 
	 * @throws Exception 
	 */
	public void sessionControl( Object control, Object value ) throws Exception;
	
	/**
	 * Notifies the chain of sessions of the specified event. Unlike query
	 * and control operations above, events are always passed up to the
	 * top to allow all sessions to notice them.
	 * 
	 * @param event a class which represents the event, possibly with
	 * parameters. The simplest event could be a string, integer,
	 * or enum, but any class instance will do (as long as some session
	 * in the chain expects it).
	 * 
	 * @throws Exception 
	 */
	public void sessionNotify( Object event ) throws Exception;

	///////////////////////
	// Well-known events //
	///////////////////////
	
	/**
	 * Session event reporting the transport is up.
	 */
	public final static String UP = "UP";
	
	/**
	 * Session event reporting the transport is down.
	 */
	public final static String DOWN = "DOWN";
}
