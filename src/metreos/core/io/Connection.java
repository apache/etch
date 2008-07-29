/* $Id$
 *
 * Created by wert on Apr 5, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package metreos.core.io;

import java.io.IOException;
import java.net.SocketAddress;
import java.net.SocketException;

import metreos.util.Monitor;
import metreos.util.Runner;
import metreos.util.RunnerHandler;
import etch.bindings.java.transport.Session;
import etch.bindings.java.transport.Transport;

/**
 * Implementation of runner which handles a network connection.
 * @param <H> the event handler type.
 */
abstract public class Connection<H extends SourceHandler>
	extends Runner implements Source<H>, RunnerHandler
{
	/////////////
	// QUERIES //
	/////////////

	/** Source query to get the local address. */
	public static final String LOCAL_ADDRESS = "LOCAL_ADDRESS";

	/** Source query to get the remote address. */
	public static final String REMOTE_ADDRESS = "REMOTE_ADDRESS";
	
	////////////
	// EVENTS //
	////////////
	
	/** Handler event that the connection is up. */
	private final static String UP = "up";
	
	/** Handler event that the connection is down. */
	private final static String DOWN = "down";

	/**
	 * Host name to specify to select listening on all interfaces.
	 * The value is "0.0.0.0".
	 */
	public static final String ALL_INTFS = "0.0.0.0";
	
	protected static String translateHost( String s )
	{
		if (s.equals( ALL_INTFS ))
			return null;
		return s;
	}

	/**
	 * Constructs the Connection.
	 *
	 * @param handler the handler for connection events.
	 */
	public Connection( H handler )
	{
		setHandler( this );
		this.handler = handler;
	}
	
	protected H handler;
	
	public H getHandler()
	{
		return handler;
	}
	
	@SuppressWarnings("unchecked")
	public void setHandler ( H handler )
	{
		this.handler = handler;
		handler.setSource( this );
	}
	
	public void started()
	{
		// ignore
	}
	
	public void stopped()
	{
		// ignore
	}

	public void exception( String what, Exception e )
	{
		try
		{
			handler.sessionNotify( e );
		}
		catch ( Exception e1 )
		{
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
	}

	@Override
	protected boolean run0( boolean first ) throws Exception
	{
		if (!openSocket( !first ))
			return false;
		
		try
		{
			setupSocket();
		}
		catch ( Exception e )
		{
			fireException( "setup", e );
			close( true );
			return true;
		}

		try
		{
			fireUp();  
			readSocket();
			return true;
		}
		catch ( SocketException e )
		{
			if ("socket closed".equalsIgnoreCase( e.getMessage() ))
				return true;
			
			fireException( "run", e );
			close( true );
			return true;
		}
		catch ( Exception e )
		{
			fireException( "run", e );
			close( true );
			return true;
		}
		finally
		{
			fireDown();
			close( false );
		}
	}

	/**
	 * Opens the socket of a connection.
	 * @param reconnect true if we are trying to reconnect, false if this
	 * is the first time.
	 * @return true if we should reconnect, false if we should stop.
	 * @throws Exception
	 */
	abstract protected boolean openSocket( boolean reconnect ) throws Exception;
	
	/**
	 * Sets up a newly opened socket. This may involve setting socket
	 * options and opening input and output streams.
	 * @throws Exception
	 */
	abstract protected void setupSocket() throws Exception;

	/**
	 * Performs the usual and customary operations on a socket, such
	 * as read or accept.
	 * @throws Exception
	 */
	abstract protected void readSocket() throws Exception;

	/**
	 * Terminates operations on the socket.
	 * @param reset true if the socket should be terminated immediately.
	 * False if the pending output data should be allowed to linger.
	 * @throws Exception
	 */
	abstract public void close( boolean reset ) throws Exception;

	/**
	 * Terminates operations on the socket. Same as close( false );
	 * @throws Exception
	 * @see #close(boolean)
	 */
	public void close() throws Exception
	{
		close( false );
	}
	
	public Object transportQuery( Object query ) throws Exception
	{
		if (query.equals( LOCAL_ADDRESS ))
			return localAddress();

		if (query.equals( REMOTE_ADDRESS ))
			return remoteAddress();
		
		if (query instanceof WaitUp)
		{
			waitUp( ((WaitUp) query).maxDelay );
			return null;
		}
		
		if (query instanceof WaitDown)
		{
			waitDown( ((WaitDown) query).maxDelay );
			return null;
		}
		
		throw new UnsupportedOperationException( "unknown query: "+query );
	}

	/**
	 * @return the local address
	 * @throws IOException
	 */
	abstract public SocketAddress localAddress() throws IOException;

	/**
	 * @return the remote address
	 * @throws IOException
	 */
	abstract public SocketAddress remoteAddress() throws IOException;
	
	public void transportControl( Object control, Object value ) throws Exception
	{
		if (control.equals( Transport.START ))
		{
			start();
			return;
		}
		
		if (control instanceof WaitUp)
		{
			waitUp( ((WaitUp) control).maxDelay );
			return;
		}

		if (control.equals( Transport.START_AND_WAIT_UP ))
		{
			start();
			waitUp( (Integer) value );
			return;
		}

		if (control.equals( Transport.STOP ))
		{
			stop();
			return;
		}
		
		if (control instanceof WaitDown)
		{
			waitDown( ((WaitDown) control).maxDelay );
			return;
		}

		if (control.equals( Transport.STOP_AND_WAIT_DOWN ))
		{
			stop();
			waitDown( (Integer) value );
			return;
		}

		throw new UnsupportedOperationException( "unknown control: "+control );
	}
	
	public void transportNotify( Object event ) throws Exception
	{
		// nothing to do.
	}

	private void fireUp() throws Exception
	{
		status.set( UP );
		handler.sessionNotify( Session.UP );
	}

	private void fireDown() throws Exception
	{
		status.set( DOWN );
		handler.sessionNotify( Session.DOWN );
	}
	
	/**
	 * Waits until the connection is up.
	 * @param maxDelay time in milliseconds to wait.
	 * @throws InterruptedException
	 */
	public void waitUp( int maxDelay ) throws InterruptedException
	{
		status.waitUntilEq( UP, maxDelay );
	}
	
	/**
	 * @param maxDelay time in milliseconds to wait.
	 * @throws InterruptedException
	 */
	public void waitDown( int maxDelay ) throws InterruptedException
	{
		status.waitUntilEq( DOWN, maxDelay );
	}
	
	private final Monitor<String> status = new Monitor<String>( "status", DOWN );
}
