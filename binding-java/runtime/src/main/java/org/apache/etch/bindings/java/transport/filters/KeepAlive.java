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

package org.apache.etch.bindings.java.transport.filters;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Validator_int;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.util.AlarmListener;
import org.apache.etch.util.AlarmManager;
import org.apache.etch.util.Resources;
import org.apache.etch.util.Timer;
import org.apache.etch.util.Todo;
import org.apache.etch.util.TodoManager;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Transport;


/**
 * KeepAlive is a message filter which periodically sends messages to the server
 * from a client to verify that the connection is still alive. Both sides
 * periodically check for the messages from the other side, noting when they
 * arrive and how long since the last one has arrived. If it has been too long
 * since a message arrived from the other side, either side will reset the
 * connection.
 * 
 * Two parameters control KeepAlive activity: delay and count. Delay is the
 * number of seconds between sending / checking. Count is the number of delay
 * intervals with no keep alive request or response before the connection is
 * reset.
 * 
 * KeepAlive is added to the transport stack (generally first) like this:
 * 
 * tcp://host:port?filter=KeepAlive&KeepAlive.delay=15&KeepAlive.count=4
 * 
 * The default values for delay and count are shown, 15 seconds and 4 messages
 * missed, respectively. If you like the defaults, you don't need to specify
 * delay and / or count.
 * 
 * Using the above defaults, the client will send the server a keep alive
 * request every 15 seconds. The client also sends its delay value and count to
 * the server. When the server receives a keep alive request, it resets its
 * delay and count values to those of the client, updates the last message time,
 * and then sends a keep alive response to the client. When the client receives
 * the keep alive response, it updates its last message time.
 * 
 * Both client and server wake up every 15 seconds and check the last message
 * time. If it is more than 60 seconds in the past (count * delay), then the
 * connection is reset.
 */
public class KeepAlive extends AbstractMessageFilter implements AlarmListener
{
	/** Time in seconds between sending message and checking for response.
	 * Default is 15 seconds. */
	public final static String DELAY = "KeepAlive.delay";
	
	/** Count of how many DELAY intervals can be missed before the connection
	 * is reset. Default is 4. */
	public final static String COUNT = "KeepAlive.count";
	
	/**
	 * @param transport
	 * @param uri
	 * @param resources
	 * @throws Exception
	 */
	public KeepAlive( TransportMessage transport, URL uri, Resources resources )
		throws Exception
	{
		super( transport, uri, resources );
		
		delay = uri.getIntegerTerm( DELAY, 15 );
		if (delay <= 0)
			throw new IllegalArgumentException( "delay <= 0" );
		
		count = uri.getIntegerTerm( COUNT, 4 );
		if (count <= 0)
			throw new IllegalArgumentException( "count <= 0" );
		
		server = (Boolean) transport.transportQuery( Transport.IS_SERVER );
		
//		Log.report( "KeepAliveInstalled",
//			"delay", delay, "count", count, "server", server );
		
		vf = (ValueFactory) resources.get( Transport.VALUE_FACTORY );

		mf_delay = new Field( "delay" );
		mf_count = new Field( "count" );
		
		mt_request = new Type( "_Etch_KeepAliveReq" );
		mt_request.putValidator( mf_delay, Validator_int.get( 0 ) );
		mt_request.putValidator( mf_count, Validator_int.get( 0 ) );
		vf.addType( mt_request );

		mt_response = new Type( "_Etch_KeepAliveResp" );
		vf.addType( mt_response );
		
		mt_request.setResult( mt_response );
	}
	
	private int delay;
	
	private int count;
	
	private final boolean server;
	
	private final ValueFactory vf;
	
	private final Field mf_delay;
	
	private final Field mf_count;
	
	private final Type mt_request;
	
	private final Type mt_response;
	
	@Override
	public String toString()
	{
		return "KeepAlive/"+transport;
	}
	
	/**
	 * @return the server operating mode.
	 */
	public boolean getServer()
	{
		return server;
	}
	
	@Override
	public boolean sessionMessage( Who sender, Message msg ) throws Exception
	{
		if (msg.isType( mt_request ) )
		{
			handleRequest( msg );
			return true;
		}
		
		if (msg.isType( mt_response ))
		{
			handleResponse( msg );
			return true;
		}
		
		return super.sessionMessage( sender, msg );
	}

	@Override
	protected boolean sessionUp()
	{
//		Log.report( "KeepAliveSessionUp", "server", server );
		up = true;
		AlarmManager.staticAdd( this, null, delay*1000 );
		tickle();
		return true;
	}
	
	@Override
	protected boolean sessionDown()
	{
//		Log.report( "KeepAliveSessionDown", "server", server );
		up = false;
		AlarmManager.staticRemove( this );
		return true;
	}
	
	private boolean up;
	
	private void handleRequest( Message msg )
	{
		if (!server)
		{
			// we're a client that got a request, likely we're talking to a
			// server that is confused.
			return;
		}
		
//		Log.report( "GotKeepAliveReq", "msg", msg );
		
		Number d = (Number) msg.get( mf_delay );
		if (d != null && d.intValue() > 0)
		{
//			Log.report( "KeepAliveResetDelay", "delay", d );
			delay = d.intValue();
		}
		
		Number c = (Number) msg.get( mf_count );
		if (c != null && c.intValue() > 0)
		{
//			Log.report( "KeepAliveResetCount", "count", c );
			count = c.intValue();
		}
		
		tickle();
		
		sendKeepAliveResp( msg );
	}

	private void handleResponse( Message msg )
	{
		if (server)
		{
			// we're a server that got a response, which means either we sent
			// a request (but we shouldn't do that if we're a server) or the
			// client is confused.
			return;
		}
		
//		Log.report( "GotKeepAliveResp", "msg", msg );
		
		tickle();
	}
	
	private void tickle()
	{
		lastTickle = Timer.getNanos();
	}
	
	private long lastTickle;
	
	private void checkTickle()
	{
		long ms = Timer.getMillisSince( lastTickle );
//		Log.report( "KeepAliveIdle", "ms", ms, "server", server );
		if (ms >= count * delay * 1000)
		{
			try
			{
//				Log.report( "KeepAliveReset", "server", server );
				session.sessionNotify( "KeepAlive resetting dead connection" );
				transport.transportControl( Transport.RESET, 0 );
			}
			catch ( Exception e )
			{
				reportError( e );
			}
		}
	}
	
	private void reportError( Exception e )
	{
		try
		{
			session.sessionNotify( e );
		}
		catch ( Exception e1 )
		{
			// what else can you do?
			e1.printStackTrace();
		}
	}
	
	private void sendKeepAliveReq()
	{
		Message msg = new Message( mt_request, vf );
		msg.put( mf_delay, delay );
		msg.put( mf_count, count );
		try
		{
//			Log.report( "SendKeepAliveReq", "msg", msg );
			transport.transportMessage( null, msg );
		}
		catch ( Exception e )
		{
			reportError( e );
		}
	}
	
	private void sendKeepAliveResp( Message msg )
	{
		Message rmsg = msg.reply();
		try
		{
//			Log.report( "SendKeepAliveResp", "rmsg", rmsg );
			transport.transportMessage( null, rmsg );
		}
		catch ( Exception e )
		{
			reportError( e );
		}
	}

	public int wakeup( AlarmManager manager, Object state, long due )
	{
//		Log.report( "KeepAliveWakeup", "server", server );
		
		if (!up)
			return 0;
		
		if (!server)
		{
			// use a Todo so as to not unnecessarily block AlarmManager.
			TodoManager.addTodo( new Todo()
			{
				public void doit( TodoManager mgr ) throws Exception
				{
					sendKeepAliveReq();
				}

				public void exception( TodoManager mgr, Exception e )
				{
					reportError( e );
				}
			} );
		}
		
		checkTickle();
		
		return delay * 1000;
	}

	/**
	 * @return the configured delay.
	 */
	public int getDelay()
	{
		return delay;
	}

	/**
	 * @return the configured count.
	 */
	public int getCount()
	{
		return count;
	}
}
