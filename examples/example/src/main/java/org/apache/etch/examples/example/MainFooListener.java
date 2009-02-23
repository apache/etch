// This file automatically generated by:
//   Apache Etch 1.1.0-incubating (LOCAL-0) / java 1.1.0-incubating (LOCAL-0)
//   Thu Feb 19 12:21:33 CST 2009
// This file is automatically created for your convenience and will not be
// overwritten once it exists! Please edit this file as necessary to implement
// your service logic.

package org.apache.etch.examples.example;

import org.apache.etch.bindings.java.support.ServerFactory;
import org.apache.etch.util.core.io.Transport;

/**
 * Main program for FooServer. This program makes a listener to accept
 * connections from MainFooClient.
 */
public class MainFooListener implements FooHelper.FooServerFactory
{
	/**
	 * Main program for FooServer.
	 * 
	 * @param args command line arguments.
	 * @throws Exception
	 */
	public static void main( String[] args ) throws Exception
	{
		// TODO Change to correct URI
		String uri = "tcp://0.0.0.0:4002";
		
		ServerFactory listener = FooHelper.newListener( uri, null,
			new MainFooListener() );

		// Start the Listener
		listener.transportControl( Transport.START_AND_WAIT_UP, 4000 );
	}

	public FooServer newFooServer( RemoteFooClient client )
	{
		return new ImplFooServer( client );
	}
}
