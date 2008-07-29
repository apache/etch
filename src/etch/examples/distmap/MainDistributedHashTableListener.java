// This file automatically generated by
 //  @EtchVersion@ (@EtchBuildTag@) / java @EtchVersion@ (@EtchBuildTag@)
 //  Mon Nov 12 15:38:33 CST 2007
package etch.examples.distmap;


import java.util.Collections;
import java.util.HashMap;
import java.util.Map;

import metreos.util.URL;
import etch.bindings.java.transport.Transport;
import etch.examples.distmap.DistributedHashTableHelper.DistributedHashTableServerFactory;


/**
 * Main implementation for DistributedHashTableListener.
 */
public class MainDistributedHashTableListener implements DistributedHashTableServerFactory
{
	/**
 	 * Main for DistributedHashTableListener.
	 * @param args
	 * @throws Exception
	 */
	public static void main(String[] args)
		throws Exception
	{	
		URL uri = new URL("tcp://0.0.0.0:4001");

		MainDistributedHashTableListener implFactory = new MainDistributedHashTableListener();
		Transport listener = DistributedHashTableHelper.newDistributedHashTableListener( uri, null, implFactory );
	
		listener.transportControl( Transport.START_AND_WAIT_UP,4000 );
	}
	
	final Map<String, Object> map =
		Collections.synchronizedMap( new HashMap<String, Object>() );

	/**
	 * Constructs the MainDistributedHashTableListener.
	 */
	public MainDistributedHashTableListener()
	{
		// Nothing to do.
	}

	/**
	 * Return a new instance of DistributedHashTableServer.
	 * @param client
	 */
	public DistributedHashTableServer newDistributedHashTableServer(DistributedHashTableClient client) throws Exception {
		System.out.println( "new ImplDistributedHashTableServer created" );
		return new ImplDistributedHashTableServer( map );
	}
}
