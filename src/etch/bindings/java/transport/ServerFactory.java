package etch.bindings.java.transport;

import etch.bindings.java.msg.ValueFactory;

public interface ServerFactory
{
	public void newServer( DefaultDeliveryService d ) throws Exception;

	public ValueFactory newValueFactory();
}
