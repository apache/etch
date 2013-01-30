/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.util.core.io;

import java.io.IOException;
import java.net.SocketAddress;
import java.net.SocketException;

import org.apache.etch.util.Monitor;
import org.apache.etch.util.Runner;
import org.apache.etch.util.RunnerHandler;
import org.apache.etch.util.Todo;
import org.apache.etch.util.TodoManager;


/**
 * Implementation of runner which handles a network connection.
 * @param <H> the event handler type.
 */
abstract public class Connection<H extends Session>
	extends Runner implements Transport<H>, RunnerHandler
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

	/**
	 * Host name to specify to select listening on all interfaces.
	 * The value is "0.0.0.0".
	 */
	public static final String ALL_INTFS = "0.0.0.0";
	
	/**
	 * @param s
	 * @return null if s is null or ALL_INTFS ("0.0.0.0").
	 */
	protected static String translateHost( String s )
	{
		if (s != null && s.equals( ALL_INTFS ))
			return null;
		return s;
	}

	/**
	 * Constructs the Connection.
	 */
	public Connection()
	{
		setHandler( this );
	}
	
	public void started()
	{
		// ignore
	}
	
	public void stopped()
	{
		// ignore
	}

	public void exception( String what, final Exception e )
	{
		TodoManager.addTodo( new Todo()
		{
			public void doit( TodoManager m ) throws Exception
			{
				session.sessionNotify( e );
			}
			
			public void exception( TodoManager mgr, Exception ex )
			{
				e.printStackTrace();
				if (ex != e)
					ex.printStackTrace();
			}
		} );
	}

	@Override
	protected boolean run0( boolean first ) throws Exception
	{
		boolean ok = openSocket( !first );
//		Log.report( "openSocketDone", "result", ok, "c", this, "t", Thread.currentThread() );
		if (!ok)
			return false;
		
		try
		{
			setupSocket();
//			Log.report( "setupSocketDone", "c", this );
		}
		catch ( Exception e )
		{
			fireException( "setup", e );
			close( true );
			return true;
		}

		try
		{
//			Log.report( "beforeFireUp", "c", this );
			fireUp();
			readSocket();
//			Log.report( "readSocketDone", "c", this );
			return true;
		}
		catch ( SocketException e )
		{
//			Log.report( "readSocketDone", "c", this, "e", e );
			
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
//			Log.report( "beforeFireDown", "c", this );
			fireDown();
			close( false );
//			Log.report( "closeDone", "c", this );
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
		if (query == LOCAL_ADDRESS)
			return localAddress();

		if (query == REMOTE_ADDRESS)
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
		if (control == START)
		{
			start();
			return;
		}

		if (control == START_AND_WAIT_UP)
		{
			start();
			waitUp( (Integer) value );
			return;
		}

		if (control == STOP)
		{
			stop();
			return;
		}

		if (control == STOP_AND_WAIT_DOWN)
		{
			stop();
			waitDown( (Integer) value );
			return;
		}
		
		if (control == RESET)
		{
			close( true );
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
		status.set( Session.UP );
		TodoManager.addTodo( new Todo()
		{
			public void doit( TodoManager m ) throws Exception
			{
				session.sessionNotify( Session.UP );
			}
			
			public void exception( TodoManager mgr, Exception e )
			{
				e.printStackTrace();
			}
		} );
	}

	private void fireDown() throws Exception
	{
		status.set( Session.DOWN );
		TodoManager.addTodo( new Todo()
		{
			public void doit( TodoManager m ) throws Exception
			{
				session.sessionNotify( Session.DOWN );
			}
			
			public void exception( TodoManager mgr, Exception e )
			{
				e.printStackTrace();
			}
		} );
	}
	
	public H getSession()
	{
		return session;
	}
	
	public void setSession( H session )
	{
		this.session = session;
	}
	
	/**
	 * The session for the connection.
	 */
	protected H session;
	
	/**
	 * Waits until the connection is up.
	 * @param maxDelay time in milliseconds to wait.
	 * @throws Exception
	 */
	public void waitUp( int maxDelay ) throws Exception
	{
		status.waitUntilEq( Session.UP, maxDelay );
	}
	
	/**
	 * @param maxDelay time in milliseconds to wait.
	 * @throws Exception
	 */
	public void waitDown( int maxDelay ) throws Exception
	{
		status.waitUntilEq( Session.DOWN, maxDelay );
	}
	
	private final Monitor<String> status = new Monitor<String>( "status", Session.DOWN );
}
