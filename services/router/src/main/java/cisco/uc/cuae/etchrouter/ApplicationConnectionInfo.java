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
package cisco.uc.cuae.etchrouter;

import java.util.Collection;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

import cisco.uc.cuae.etchrouter.EtchRouter.EtchRouterException;
import cisco.uc.cuae.etchrouter.EtchRouter.PluginServiceState;
import cisco.uc.cuae.etchrouter.plugin.PluginGroup;
import cisco.uc.cuae.etchrouter.plugin.PluginMember;
import cisco.uc.cuae.etchrouter.plugin.PluginMemberConnection;
import cisco.uc.cuae.etchrouter.utils.ApplicationAttributes;
import etch.bindings.java.msg.Direction;
import etch.bindings.java.msg.Type;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class ApplicationConnectionInfo
{	
	private static final Logger _LOGGER = Logger.getLogger( ApplicationConnectionInfo.class.getName());
	
	private ConnectionStackInfo _appClientConnInfo = null;
	
	private Map<Type, PluginGroup> _method2pluginGroupMap = null;
	
	private Map<PluginGroup, PluginMemberConnection> _pluginGroup2PluginMemberConnMap = null;
	
	private Map<String, PluginGroup> _pluginGroups = null;
	
	EtchRouterManager _etchRouterManager = null;
	
	private String _applicationParam = null;
	
	private ApplicationAttributes _applicationAttrs = null;
	
	// plugin name to statuc map:
	private Map<String, PluginServiceState> _previousStateSentMap = new HashMap<String, PluginServiceState>();
	
	/**
	 * 
	 * @param appClientConnInfo
	 * @param routerMgr
	 * @param initMethod
	 * @throws Exception
	 */
	public ApplicationConnectionInfo( ConnectionStackInfo appClientConnInfo, EtchRouterManager routerMgr, Type initMethod) throws Exception
	{
		initFields(null, appClientConnInfo, routerMgr);
		loadPluginGroupByMethodType( initMethod );
	}
	
	/**
	 * 
	 * @param methodType
	 * @throws Exception
	 */
	public void loadPluginGroupByMethodType( Type methodType ) throws Exception
	{
		PluginGroup pluginGrp = _etchRouterManager.getPluginGroup( methodType );
		if (pluginGrp==null)
			throw new EtchRouterException(1, String.format( "The method \"%s\" is not an installed plugin API", methodType.getName() ));
		if (_pluginGroups.get( pluginGrp.getName() )==null)
		{
			_LOGGER.log( Level.INFO, "Loading plugin group \"{0}\" for client connection {1}", new Object[] { pluginGrp.getName(), _appClientConnInfo } );
			Collection<Type> methods = pluginGrp.getInstallInfo().getMethods();
			synchronized(_method2pluginGroupMap)
			{
				for (Type method : methods)
					_method2pluginGroupMap.put( method, pluginGrp );
			}
			synchronized(_pluginGroups)
			{
				if (_pluginGroups.get( pluginGrp.getName() )==null)
					_pluginGroups.put( pluginGrp.getName(), pluginGrp );
			}
			pluginGrp.addApplicationConnection( this );
			initPluginConnectionIfNotConnected(pluginGrp);
		}
	}
	
	/**
	 * 
	 * @param appParam
	 * @param appInstallInfo
	 * @param appClientConnInfo
	 * @param routerMgr
	 * @throws Exception
	 */
	public ApplicationConnectionInfo( String appParam, ApplicationInstallInfo appInstallInfo, ConnectionStackInfo appClientConnInfo, EtchRouterManager routerMgr )
		throws Exception
	{
		initFields(appParam, appClientConnInfo, routerMgr);
		initPluginGroups(appInstallInfo);
		connectToPlugins();
	}
	
	public boolean isAnonymous()
	{
		return _appClientConnInfo.getPluginOrAppName()==null;
	}
	
	public void setApplicationName( String applicationName )
	{
		_appClientConnInfo.setPluginOrAppName( applicationName );
	}
	
	private void initFields(String appParam, ConnectionStackInfo appClientConnInfo, EtchRouterManager routerMgr) throws Exception
	{
		_appClientConnInfo = appClientConnInfo;
		setApplicationParam( appParam );
		_method2pluginGroupMap = new HashMap<Type, PluginGroup>();
		_pluginGroup2PluginMemberConnMap = new HashMap<PluginGroup, PluginMemberConnection>();
		_etchRouterManager = routerMgr;
		_pluginGroups = new LinkedHashMap<String, PluginGroup>();
	}
	
	public void setApplicationParam( String appParam ) throws Exception
	{
		_applicationParam = appParam;
		try
		{
			_applicationAttrs = new ApplicationAttributes( _applicationParam );
		}
		catch (Exception e)
		{
			String msg = String.format( "Invalid application parameter \"%s\"", _applicationParam);
			_LOGGER.log( Level.SEVERE, msg, e );
			throw new EtchRouterException(1, msg);
		}
	}
	
	private void initPluginGroups(ApplicationInstallInfo appInstallInfo) throws Exception
	{
		if (appInstallInfo==null)
		{
			//Allow an anonymous application to be registered :
			return;
			//throw new EtchRouterException(1, String.format( "Application '%s' is not installed with Etch Router manager.", appInstallInfo.getName() ));
		}
		Collection<Type> appMethods = appInstallInfo.getMethods();
		for (Type method : appMethods)
		{
			if (method.getDirection()!=Direction.CLIENT)
			{
				PluginGroup pluginGroup = _etchRouterManager.getPluginGroup( method );
				if (pluginGroup==null)
				{
					if (!_etchRouterManager.isLocalMethodType( method ))
						throw new EtchRouterException(1, String.format( "Cannot find an installed plugin prividing API method service: '%s'", method.getName() ));
				}
				else
				{
					_method2pluginGroupMap.put( method, pluginGroup );
					if ( _pluginGroups.get( pluginGroup.getName() )==null)
					{
						_pluginGroups.put( pluginGroup.getName(), pluginGroup );
						pluginGroup.addApplicationConnection( this );
					}
				}
			}
		}
	}
	
	private void connectToPlugins()
	{
		for (PluginGroup pluginGrp : _pluginGroups.values())
		{
			InitPluginConnectionRunner runner = new InitPluginConnectionRunner(pluginGrp);
			new Thread(runner).start();
		}
	}
	
	private class InitPluginConnectionRunner implements Runnable
	{
		private PluginGroup _pluginGrp = null;
		private InitPluginConnectionRunner(PluginGroup pluginGrp)
		{
			_pluginGrp = pluginGrp;
		}
		public void run()
		{
			initPluginConnection( _pluginGrp );
		}
	}
	
	/**
	 * 
	 * @param pluginGrp
	 */
	public synchronized void initPluginConnectionIfNotConnected( PluginGroup pluginGrp )
	{
		if (isConnectedToPluginGroup( pluginGrp )) return;
		initPluginConnection( pluginGrp );
	}
	
	private void sendEventToClientConnection( String pluginName, String detail, PluginServiceState state)
	{
		synchronized (_previousStateSentMap)
		{
			// do not send duplicate plugin state change message to client:
			if (_previousStateSentMap.get( pluginName )==state) return;
			_previousStateSentMap.put( pluginName, state);
		}
		try
		{
			_etchRouterManager.sentNotifyMsgToConnection( String.format( "Plugin: %s, Status: %s, Detail: %s", pluginName, state, detail  ), this._appClientConnInfo);
		}
		catch (Exception e)
		{
			_LOGGER.log( Level.INFO, "Got Exception", e);
		}
		if (this.isAnonymous())
		{
			_LOGGER.log( Level.FINEST, "No plugin service {0} event sent to anonymous connection: {1}", new Object[]{ state, _appClientConnInfo} );
		}
		else
		{
			try
			{
				_etchRouterManager.callAppClientMethod_pluginServiceStateChange( pluginName, state, detail, this._appClientConnInfo );
			}
			catch (Exception ee)
			{
				_LOGGER.log( Level.SEVERE, "Got Exception: ", ee);
			}
		}
	}
	
	/**
	 * 
	 * @param pluginGrp
	 */
	public synchronized void initPluginConnection( PluginGroup pluginGrp )
	{
		PluginMember member = null;
		try
		{
			member = pluginGrp.getActiveMember( _applicationAttrs );
		}
		catch (Exception e)
		{
			String detail = String.format( "The plugin group '%s' failed to return an active plugin instance",  pluginGrp.getName());
			if (_LOGGER.isLoggable( Level.FINEST ))
				_LOGGER.log( Level.FINEST, detail, e);
			else 
				_LOGGER.log( Level.INFO, detail + " : " + e.getLocalizedMessage());
			sendEventToClientConnection( pluginGrp.getName(), detail, PluginServiceState.DOWN);
			return;
		}
		ERRemoteServer remoteServer = null;
		try
		{
			remoteServer = EtchRouterManagerHelper.newServer( _etchRouterManager, member.getUrl(), null, pluginGrp.getName() );
		}
		catch (Exception e)
		{
			String detail = String.format( "Failed to create a remote server handle with url '%s' for plugin group '%s'", member.getUrl(), pluginGrp.getName());
			if (_LOGGER.isLoggable( Level.FINEST ))
				_LOGGER.log( Level.FINEST, detail, e);
			else 
				_LOGGER.log( Level.INFO, detail + " : " + e.getLocalizedMessage());
			sendEventToClientConnection( pluginGrp.getName(), detail, PluginServiceState.DOWN);
			return;
		}
		PluginMemberConnection instConn = new PluginMemberConnection( member, remoteServer );

		_LOGGER.log( Level.INFO, "Connecting to remote server: {0}, application connection: {1} ", new Object[] { remoteServer.toString(), this._appClientConnInfo } );
		try
		{
			remoteServer._startAndWaitUp( _etchRouterManager.getConnectionStartMaxDelay() );
		}
		catch (Exception e)
		{
			String detail = String.format( "The remote server failed to start, url is '%s', plugin group is '%s'", member.getUrl(), pluginGrp.getName());
			if (_LOGGER.isLoggable( Level.FINEST ))
				_LOGGER.log( Level.FINEST, detail, e);
			else 
				_LOGGER.log( Level.INFO, detail + " : " + e.getLocalizedMessage());
			sendEventToClientConnection( pluginGrp.getName(), detail, PluginServiceState.DOWN);
			return;
		}
		String detail = String.format( "Successfully connected to plugin '%s' for application connection '%s', remote plugin connection is '%s'",  pluginGrp.getName(), this._appClientConnInfo.toString(), remoteServer.toString());
		_LOGGER.log( Level.INFO, detail);
		sendEventToClientConnection( pluginGrp.getName(), detail, PluginServiceState.UP);
		member.addMemberConnection( instConn );
		_pluginGroup2PluginMemberConnMap.put( pluginGrp, instConn );
		_etchRouterManager.addAppConnectionInfo( remoteServer.getConnectionStackInfo(), this );
	}
	
	public synchronized boolean isConnectedToPluginGroup( PluginGroup pluginGroup )
	{
		return _pluginGroup2PluginMemberConnMap.containsKey( pluginGroup );
	}
	
	public PluginGroup getPluginGroupByName( String pluginGrpName )
	{
		return _pluginGroups.get( pluginGrpName );
	}
	
	public Collection<PluginGroup> getReferencedPluginGroups()
	{
		return _pluginGroups.values();
	}
	
	public synchronized void stopAllPluginConnections()
	{
		for (PluginGroup pluginGrp : _pluginGroups.values())
			stopPluginConnection( pluginGrp, false );
	}
	
	public synchronized void stopPluginConnection( PluginGroup pluginGrp, boolean sendEventToApp )
	{
		PluginMemberConnection instConn = _pluginGroup2PluginMemberConnMap.remove( pluginGrp );
		if ( instConn != null)
		{
			instConn.getMember().removeMemberConnection( instConn );
			ERRemoteServer server = instConn.getRemoteServer();
			_etchRouterManager.removeAppConnectionInfo( server.getConnectionStackInfo() );
			_LOGGER.log( Level.INFO, "Stopping remote server: {0} for application connection: {1}",  new Object[] { server.toString(), this._appClientConnInfo } );
			try
			{
				server._stopAndWaitDown( _etchRouterManager.getConnectionStopMaxDelay() );
			}
			catch (Exception e)
			{
				_LOGGER.log( Level.FINE, "Got Exception: ", e);
			}
			if (sendEventToApp)
			{
				sendEventToClientConnection( pluginGrp.getName(), String.format( "The remote server disconnected: %s", server ), PluginServiceState.DOWN);
			}
		}
	}
	
	/**
	 * Getter
	 * 
	 * @return
	 */
	public ConnectionStackInfo getApplicationClientConnectionStackInfo()
	{
		return _appClientConnInfo;
	}
	
	/**
	 * 
	 * @param method
	 * @return
	 */
	public PluginMemberConnection getPluginConnectionByMethod( Type method )
	{
		PluginGroup grp = _method2pluginGroupMap.get( method );
		if (grp==null) return null;
		return _pluginGroup2PluginMemberConnMap.get( grp );
	}
	
	/**
	 * 
	 * @param pluginName
	 * @return
	 */
	public synchronized PluginMemberConnection getPluginConnectionByPluginGroup( PluginGroup pluginGrp )
	{
		if (pluginGrp==null) return null;
		return _pluginGroup2PluginMemberConnMap.get( pluginGrp );
	}
	
}
