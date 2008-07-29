// -----------------------------------------------------------
// File: SampleServerImpl.java
// -----------------------------------------------------------

import java.rmi.Naming;
import java.rmi.RMISecurityManager;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;

/**
 * Sample server for java rmi performance testing.
 */
public class SampleServerImpl extends UnicastRemoteObject implements
	SampleServer
{
	/** serialVersionUID */
	private static final long serialVersionUID = 1L;

	SampleServerImpl() throws RemoteException
	{
		super();
	}

	public int add( int a, int b ) throws RemoteException
	{
		return a + b;
	}
	
	public int sum( int[] values ) throws RemoteException
	{
		int sum = 0;
		for (int value: values)
			sum += value;
		return sum;
	}
	
	public void report( int code, String msg ) throws RemoteException
	{
		// ignore.
	}

	/**
	 * @param args
	 * @throws Exception
	 */
	public static void main( String args[] ) throws Exception
	{
		System.setSecurityManager( new RMISecurityManager() );

		// create a local instance of the object
		SampleServerImpl Server = new SampleServerImpl();

		// put the local instance in the registry
		Naming.rebind( "SAMPLE-SERVER", Server );

		System.out.println( "Server waiting....." );
	}
}
