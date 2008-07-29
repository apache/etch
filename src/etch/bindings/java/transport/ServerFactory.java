package etch.bindings.java.transport;

import etch.bindings.java.support.DeliveryService;

/**
 * Interface to use for constructing new server instances by
 * TransportHelper.
 */
public interface ServerFactory
{
	/**
	 * @param d the delivery service to use with the new server.
	 * @throws Exception
	 */
	public void newServer( DeliveryService d ) throws Exception;
}
