/* $Id$
 *
 * Copyright 2009-2010 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */
package org.apache.etch.services.router;

import java.util.logging.Level;
import java.util.logging.Logger;

import org.apache.etch.bindings.java.msg.Direction;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.bindings.java.transport.TransportMessage;
import org.apache.etch.services.router.ConnectionStackInfo.ConnectionType;
import org.apache.etch.services.router.EtchRouter.EtchRouterException;
import org.apache.etch.services.router.plugin.PluginGroup;
import org.apache.etch.services.router.plugin.PluginMemberConnection;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class EtchRouterMessageFilter implements TransportMessage,
	SessionMessage
{
	
	private static final Logger _LOGGER = Logger.getLogger( EtchRouterMessageFilter.class.getName());
	
	private EtchRouterManager _etchRouterManager = null;
	
	private final TransportMessage _transport;
	
	private SessionMessage _session = null;

	/**
	 * 
	 * @param transport
	 * @param uri
	 * @param resources
	 */
	public EtchRouterMessageFilter( TransportMessage transport, URL uri,
		Resources resources )
	{
		this._transport = transport;
		_etchRouterManager = (EtchRouterManager)resources.get( EtchRouterManager._ETCH_ROUTER_MANAGER );
		transport.setSession( this );
	}

	@Override
	public String toString()
	{
		return String.format( "EtchRouterMessageFilter/%s", _transport);
	}
	
	public void transportMessage( Who recipient, Message msg ) throws Exception
	{
		_transport.transportMessage( recipient, msg );
	}

	public SessionMessage getSession()
	{
		return _session;
	}

	public void setSession( SessionMessage session )
	{
		_session = session;
	}

	public void transportControl( Object control, Object value )
		throws Exception
	{
		_transport.transportControl( control, value );
	}

	public void transportNotify( Object event ) throws Exception
	{
		_transport.transportNotify( event );
	}

	public Object transportQuery( Object query ) throws Exception
	{
		return _transport.transportQuery( query );
	}

	public boolean sessionMessage( Who sender, Message msg ) throws Exception
	{
    	ConnectionStackInfo connStackInfo = _etchRouterManager.getConnectionStackInfo( this );
    	if (connStackInfo==null)
    	{
    		_LOGGER.log( Level.SEVERE, "No connection stack info is saved with EtchRouter manager for {0}", this );
    		return false;
    	}
		Type methodType = msg.type();
		ConnectionType connType = connStackInfo.getConnectionType();
		if (_etchRouterManager.isLocalMethodType( methodType ))
		{
			Long msgid = msg.getInReplyTo();
			// the upper level mailbox manager will handle this message if
			// it's a reply message
			if (msgid == null)
			{
				_etchRouterManager.handleLocalMethodMessage( sender, connStackInfo, msg );
			}
		}
		else if( connType==ConnectionType.APP_CLIENT_CONN )
		{
			try
			{
				handleMessageFromAppClientConnection( sender, msg, connStackInfo, methodType );
			}
			catch (Exception e)
			{
				returnException( sender, msg, e, connStackInfo);
			}
		}
		else if ( connType==ConnectionType.PLUGIN_SERVER_CONN )
		{
			//from plugin-server -> app-client:
			ApplicationConnectionInfo appConnInfo = _etchRouterManager.getAppConnectionInfoByPluginServerStackInfo( connStackInfo );
			ConnectionStackInfo appClientStackInfo = appConnInfo.getApplicationClientConnectionStackInfo();
			appClientStackInfo.getMessageFilter().transportMessage( sender, msg );
		}
		else if ( connType==null && methodType.getDirection()!=Direction.CLIENT)
		{
			try
			{
				_etchRouterManager.registerAnonymousApplication( connStackInfo, methodType );
				handleMessageFromAppClientConnection( sender, msg, connStackInfo, methodType );
			}
			catch (Exception e)
			{
				returnException( sender, msg, e, connStackInfo);
			}
		}
		return _session.sessionMessage( sender, msg );
	}

	private void returnException( Who sender, Message origMsg, Exception e, ConnectionStackInfo connStackInfo)
	{
		Type methodType = origMsg.type();
		_LOGGER.log( Level.INFO, "Got Exception, original message type is "+methodType.getName()+": ", e);
		Message rmsg = (methodType.getResult()==null) ? null : origMsg.reply();
		if (rmsg==null) return;
		if (!(e instanceof RuntimeException)) e = new RuntimeException( e );
		rmsg.put( DynamicValueFactory._mf_result, e);
		try
		{
			connStackInfo.getDeliveryService().transportMessage( sender, rmsg );
		}
		catch (Exception ee)
		{
			_LOGGER.log( Level.SEVERE, "Got Exception: ", ee);
		}
	}
	
	
	private void handleMessageFromAppClientConnection(Who sender, Message msg, ConnectionStackInfo connStackInfo, Type methodType ) throws Exception
	{
		//from app-client -> plugin:
		ApplicationConnectionInfo appConnInfo = _etchRouterManager.getAppConnectionInfoByAppClientStackInfo( connStackInfo );
		// make sure the type info for this method is loaded with the app connection:
		try
		{
			appConnInfo.loadPluginGroupByMethodType( methodType );
		}
		catch (Exception e)
		{
			// ignore this exception for we don't care whether it's a server direction'ed message
		}
		PluginMemberConnection pluginConn = appConnInfo.getPluginConnectionByMethod( methodType );
		if (pluginConn==null)
		{
			PluginGroup pluginGrp = _etchRouterManager.getPluginGroup( methodType );
			if (pluginGrp!=null)
			{
				//No plugin connection is available for this API call:
				throw new EtchRouterException(1, String.format( "Cannot call API '%s' because the plugin connection is not established", methodType.getName() ));
			}
			else
			{
				_LOGGER.log( Level.INFO, "Not handled message from client connection: {0}", msg );
			}
		}
		else
		{
			ERRemoteServer remoteServer = pluginConn.getRemoteServer();
			ConnectionStackInfo serverStackInfo = remoteServer.getConnectionStackInfo();
			serverStackInfo.getMessageFilter().transportMessage( sender, msg );
		}
	}
	
	public void sessionControl( Object control, Object value ) throws Exception
	{
		_session.sessionControl( control, value );
	}

	public void sessionNotify( Object event ) throws Exception
	{
		if (event instanceof Exception)
		{
			_LOGGER.log( Level.FINER, "Got an exception in sessionNotify", (Exception)event);
		}
		_session.sessionNotify( event );
	   	ConnectionStackInfo connStackInfo = _etchRouterManager.getConnectionStackInfo( this );
    	if (connStackInfo==null)
    	{
    		_LOGGER.log( Level.SEVERE, "No connection stack info is saved with EtchRouter manager for {0}", this );
    	}
    	else
    	{
    		_etchRouterManager.sessionNotify( event, connStackInfo );
    	}
	}

	public Object sessionQuery( Object query ) throws Exception
	{
		return _session.sessionQuery( query );
	}

}
