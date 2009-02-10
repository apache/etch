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
import org.apache.etch.bindings.java.support.Validator_boolean;
import org.apache.etch.bindings.java.support.Validator_string;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.util.Log;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Transport;


/**
 * PwAuth is a message filter which watches for Session.UP and attempts to
 * authenticate the user by sending a message to the server with the name and
 * password in it. If the server responds, the session is notified with the
 * user name. If the server responds negatively, the session is notified of the
 * failure (using a PwAuthFail event). A transport control may also be sent to
 * force a login or logout or to set the name or password. The name and password
 * are initialized from the uri, or prompted for if not specified.
 */
public class PwAuth extends AbstractMessageFilter
{
	/**
	 * client sessionQuery: return user name to use for authentication.
	 * transportQuery: return the current user name (on server it has been validated).
	 * client transportControl: set user name to use for authentication.
	 */
	public static final String USER = "PwAuth.user";

	/**
	 * client sessionQuery: return password to use for authentication.
	 * transportQuery: return the current password (on server it has been validated).
	 * client transportControl: set password to use for authentication.
	 */
	public static final String PASSWORD = "PwAuth.password";

	/**
	 * client transportControl: send authentication request
	 */
	private static final String AUTHENTICATE = "PwAuth.authenticate";
	
	/**
	 * @param transport
	 * @param uri
	 * @param resources
	 * @throws Exception
	 */
	public PwAuth( TransportMessage transport, URL uri, Resources resources )
		throws Exception
	{
		super( transport, uri, resources );
		server = (Boolean) transport.transportQuery( Transport.IS_SERVER );
		
		if (!server)
		{
			user = uri.getUser();
			password = uri.getPassword();
		}
		
//		Log.report( "KeepAliveInstalled",
//			"delay", delay, "count", count, "server", server );
		
		vf = (ValueFactory) resources.get( Transport.VALUE_FACTORY );

		mt_request = new Type( "_Etch_PwAuthReq" );
		mt_response = new Type( "_Etch_PwAuthResp" );
		
		vf.addType( mt_request );
		vf.addType( mt_response );
		
		mf_user = new Field( "user" );
		mf_password = new Field( "password" );
		mf_ok = new Field( "ok" );
		mf_status = new Field( "status" );
		
		mt_request.putValidator( mf_user, Validator_string.get( 0 ) );
		mt_request.putValidator( mf_password, Validator_string.get( 0 ) );
		mt_request.setResult( mt_response );
		mt_request.lock();
		
		mt_response.putValidator( mf_ok, Validator_boolean.get( 0 ) );
		mt_response.putValidator( mf_status, Validator_string.get( 0 ) );
		mt_response.lock();
	}
	
	private String user;
	
	private String password;
	
	private final boolean server;
	
	private final ValueFactory vf;
	
	private final Field mf_user;
	
	private final Field mf_password;
	
	private final Field mf_ok;
	
	private final Field mf_status;
	
	private final Type mt_request;
	
	private final Type mt_response;
	
	@Override
	public String toString()
	{
		return "PwAuth/"+transport;
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
			handlePwAuthReq( sender, msg );
			return true;
		}
		
		if (msg.isType( mt_response ))
		{
			handlePwAuthResp( sender, msg );
			return true;
		}
		
		return super.sessionMessage( sender, msg );
	}

	@Override
	protected boolean sessionUp() throws Exception
	{
		if (!server)
		{
			// client: send authentication request.
			sendPwAuthReq();
		}
		
		return true;
	}

	@Override
	protected boolean sessionDown() throws Exception
	{
		if (server)
		{
			// make sure that server session's idea of user is cleared.
			sessionNotifyUser( null, null );
		}
		
		return true;
	}

	private void sendPwAuthReq() throws Exception
	{
		if (user == null)
		{
			user = (String) session.sessionQuery( USER );
			if (user == null)
				throw new IllegalStateException( "PwAuth: user == null" );
		}
		
		if (password == null)
		{
			password = (String) session.sessionQuery( PASSWORD );
			if (password == null)
				throw new IllegalStateException( "PwAuth: password == null" );
		}
		
		Message msg = new Message( mt_request, vf );
		msg.put( mf_user, user );
		msg.put( mf_password, password );
		transport.transportMessage( null, msg );
	}
	
	private void handlePwAuthReq( Who sender, Message msg ) throws Exception
	{
		if (!server)
		{
			// we're a client that got a request, likely we're talking to a
			// server that is confused.
			return;
		}
		
		Log.report( "GotPwAuthReq", "sender", sender, "msg", msg );
		
		String u = (String) msg.get( mf_user );
		String p = (String) msg.get( mf_password );
		
		try
		{
			OkStatus as = (OkStatus) sessionQuery( new UserPassword( u, p ) );
			
			if (as.ok)
				sessionNotifyUser( u, p );
			else
				sessionNotifyUser( null, null );
			
			sendPwAuthResp( sender, msg, as.ok, as.status );
		}
		catch ( Exception e )
		{
			sessionNotifyUser( null, null );
			sendPwAuthResp( sender, msg, false, e.toString() );
			throw e;
		}
	}
	
	private void sendPwAuthResp( Who recipient, Message msg, boolean result, String status )
		throws Exception
	{
		Message rmsg = msg.reply();
		rmsg.put( mf_ok, result );
		rmsg.put( mf_status, status );
		transport.transportMessage( recipient, rmsg );
	}

	private void handlePwAuthResp( Who sender, Message msg ) throws Exception
	{
		if (server)
		{
			// we're a server that got a response, likely we're talking to a
			// client that is confused.
			return;
		}
		
		Log.report( "GotPwAuthResp", "sender", sender, "msg", msg );
		
		boolean result = (Boolean) msg.get( mf_ok );
		String status = (String) msg.get( mf_status );
		
		session.sessionNotify( new OkStatus( result, status ) );
	}
	
	@Override
	public Object transportQuery( Object query ) throws Exception
	{
		if (query == USER)
			return user;
		
		if (query == PASSWORD)
			return password;
		
		return super.transportQuery( query );
	}
	
	@Override
	public void transportControl( Object control, Object value )
		throws Exception
	{
		if (control == USER)
		{
			checkNotServer();
			user = (String) value;
			return;
		}
		
		if (control == PASSWORD)
		{
			checkNotServer();
			password = (String) value;
			return;
		}
		
		if (control == AUTHENTICATE)
		{
			checkNotServer();
			sendPwAuthReq();
			return;
		}
		
		super.transportControl( control, value );
	}

	private void checkNotServer()
	{
		if (server)
			throw new IllegalStateException( "control not permitted by server" );
	}
	
	@SuppressWarnings("hiding")
	private void sessionNotifyUser( String user, String password )
		throws Exception
	{
		this.user = user;
		this.password = password;
		session.sessionNotify( new UserPassword( user, password ) );
	}

	/**
	 * Server session query to get AuthOkStatus, also server session event
	 * to report changes in authentication status.
	 */
	public static class UserPassword
	{
		/**
		 * Constructs the CheckAuth.
		 *
		 * @param user name to check.
		 * @param password of the user.
		 */
		public UserPassword( String user, String password )
		{
			this.user = user;
			this.password = password;
		}
		
		@Override
		public String toString()
		{
			return String.format( "AuthUserPassword( %s )", user );
		}
		
		/** The user name to check */
		public final String user;
		
		/** The password of the user */
		public final String password;
	}
	
	/**
	 * Server session query response to AuthUserPassword.
	 */
	public static class OkStatus
	{
		/**
		 * Constructs the AuthOkStatus.
		 *
		 * @param ok true if user / password valid.
		 * @param status message related to ok.
		 */
		public OkStatus( boolean ok, String status )
		{
			this.ok = ok;
			this.status = status;
		}
		
		@Override
		public String toString()
		{
			return String.format( "AuthOkStatus( %s, %s )", ok, status );
		}
		
		/** true if the user / password matched */
		public final boolean ok;
		
		/** message related to ok */
		public final String status;
	}
}
