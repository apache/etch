package metreos.core.io;

import java.net.SocketAddress;

import metreos.core.Who;
import metreos.util.FlexBuffer;
import metreos.util.Todo;
import metreos.util.TodoManager;

/**
 * A packet source which delivers to another packet source.
 */
public class Loopback implements PacketSource
{
	/**
	 * Constructs the packet source.
	 * @throws Exception
	 */
	public Loopback() throws Exception
	{
		tm = new TodoManager(10, 10, 1, 1, 0, 99 );
		tm.start();
	}
	
	private TodoManager tm;
	
	/**
	 * @return the loopback packet source that we deliver our outgoing
	 * packets to.
	 */
	public Loopback getLoopback()
	{
		return loopback;
	}
	
	/**
	 * Sets the loopback packet source that we deliver our outgoing
	 * packets to.
	 * @param loopback
	 */
	public void setLoopback( Loopback loopback )
	{
		this.loopback = loopback;
	}

	private Loopback loopback;
	
	public PacketHandler getHandler()
	{
		return handler;
	}

	public void setHandler( PacketHandler handler )
	{
		this.handler = handler;
	}
	
	/**
	 * Handler for packets from the other side.
	 */
	PacketHandler handler;
	
	public int headerSize()
	{
		return 0;
	}

	public void packet( Who recipient, FlexBuffer buf ) throws Exception
	{
		loopback.deliver( buf.getAvailBytes() );
	}
	
	/**
	 * @param buf a packet from our loopback partner. we deliver this
	 * to our handler on a thread. if null, our partner has shutdown
	 * output.
	 * @throws InterruptedException
	 */
	private void deliver( final byte[] buf ) throws InterruptedException
    {
		tm.add( new Todo()
		{
			public void doit( TodoManager mgr ) throws Exception
            {
	            handler.packet( null,
	            	buf != null ? new FlexBuffer( buf ) : null );
            }

			public void exception( TodoManager mgr, Exception e )
            {
	            e.printStackTrace();
            }
		} );
    }

	public void close( boolean reset ) throws Exception
	{
		// ignore.
	}

	public SocketAddress localAddress() throws Exception
	{
		// ignore.
		return null;
	}

	public SocketAddress remoteAddress() throws Exception
	{
		// ignore.
		return null;
	}

	public void shutdownInput() throws Exception
	{
		// ignore.
	}

	public void shutdownOutput() throws Exception
	{
		loopback.deliver( null );
	}

	public void stop() throws Exception
	{
		// ignore.
	}

	public void oobNotifySource( String event, Object... args )
	{
		// TODO Auto-generated method stub
		
	}

	public void transportControl( Object control, Object value ) throws Exception
	{
		// TODO Auto-generated method stub
		
	}

	public void transportNotify( Object event ) throws Exception
	{
		// TODO Auto-generated method stub
		
	}

	public Object transportQuery( Object query ) throws Exception
	{
		// TODO Auto-generated method stub
		return null;
	}
}
