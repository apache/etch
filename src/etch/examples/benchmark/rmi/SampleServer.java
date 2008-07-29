import java.rmi.*;

/**
 * Interface to sample server for java rmi performance testing.
 */
public interface SampleServer extends Remote
{
	/**
	 * @param a
	 * @param b
	 * @return the sum of a and b.
	 * @throws RemoteException
	 */
	public int add( int a, int b ) throws RemoteException;
	
	/**
	 * @param values
	 * @return the sum of the values.
	 * @throws RemoteException
	 */
	public int sum( int[] values ) throws RemoteException;
	
	/**
	 * @param code
	 * @param msg
	 * @throws RemoteException
	 */
	public void report( int code, String msg ) throws RemoteException;
}
