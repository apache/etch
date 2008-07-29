package metreos.core.io;

import java.net.InetAddress;

import metreos.core.Who;

/**
 * Who for an inet address / port.
 */
public class InetWho implements Who
{
	/**
	 * @param addr
	 * @param port
	 */
	public InetWho( InetAddress addr, int port )
	{
		this.addr = addr;
		this.port = port;
	}
	
	private final InetAddress addr;
	
	private final int port;

	/**
	 * @return the address of who.
	 */
	public InetAddress getInetAddress()
	{
		return addr;
	}

	/**
	 * @return the port of who.
	 */
	public int getPort()
	{
		return port;
	}

	/**
	 * @param addr
	 * @param port
	 * @return true if the specified addr and port match this who.
	 */
	public boolean matches( InetAddress addr, int port )
	{
		return addr.equals( addr ) && port == port;
	}
}