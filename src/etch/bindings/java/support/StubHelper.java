package etch.bindings.java.support;

import etch.bindings.java.msg.Message;
import etch.util.core.Who;

/**
 * Generated code to dispatch method from message.
 * @param <T> the service interface type.
 */
abstract public class StubHelper<T>
{
	/**
	 * Constructs a StubHelper which uses synchronous mode to dispatch
	 * method from message.
	 */
	public StubHelper()
	{
		// nothing to do.
	}
	
	/**
	 * Dispatches the method from message.
	 * @param svc
	 * @param obj
	 * @param sender
	 * @param msg
	 * @throws Exception
	 */
	abstract public void run( DeliveryService svc, T obj, Who sender, Message msg )
		throws Exception;
}
