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

import cisco.uc.cuae.etchrouter.utils.UniqueKeyGenerator;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.transport.MailboxManager;
import org.apache.etch.util.core.io.Connection;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class ConnectionStackInfo
{
	public enum ConnectionType 
	{
		APP_CLIENT_CONN, PLUGIN_MONITOR_CONN, PLUGIN_SERVER_CONN
	}
	
	private String _pluginOrAppName = null;

	private MailboxManager _mailboxMgr = null;
	private DeliveryService _deliveryService = null;
	private ValueFactory _valueFactory = null;
	
	private EtchRouterMessageFilter _messageFilter = null;
	
	private ConnectionType _connectionType = null;

	private String _id = null;
	
	private String _cachedLocalAddress = null;
	
	private String _cachedRemoteAddress = null;

	/**
	 * Constructor
	 * 
	 * @param messageFilter
	 * @param mailboxMgr
	 * @param deliveryService
	 * @param valueFactory
	 */
	public ConnectionStackInfo(EtchRouterMessageFilter messageFilter, MailboxManager mailboxMgr, DeliveryService deliveryService, 
		ValueFactory valueFactory )
	{
		_id = UniqueKeyGenerator.getInstance().generateKey( null );
		_messageFilter = messageFilter;
		_mailboxMgr = mailboxMgr;
		_deliveryService = deliveryService;
		_valueFactory = valueFactory;
	}
	
	public ConnectionType getConnectionType()
	{
		return _connectionType;
	}
	
	public void setConnectionType(ConnectionType connType, String pluginOrAppName)
	{
		_connectionType = connType;
		_pluginOrAppName = pluginOrAppName;
	}
	
	public void setPluginOrAppName(String pluginOrAppName)
	{
		_pluginOrAppName = pluginOrAppName;
	}
	
	public String getPluginOrAppName()
	{
		return _pluginOrAppName;
	}
	
	public EtchRouterMessageFilter getMessageFilter()
	{
		return _messageFilter;
	}
	
	/**
	 * Getter
	 * 
	 * @return
	 */
	public MailboxManager getMailboxMgr()
	{
		return _mailboxMgr;
	}

	/**
	 * Getter
	 * 
	 * @return
	 */
	public DeliveryService getDeliveryService()
	{
		return _deliveryService;
	}

	/**
	 * Getter
	 * 
	 * @return
	 */
	public ValueFactory getValueFactory()
	{
		return _valueFactory;
	}
	
	@Override
	public String toString()
	{
		//return String.format( "%s[ID:%s, name:%s, conn-type:%s, delivery-service:%s]", this.getClass().getSimpleName(), _id, _pluginOrAppName, _connectionType, _deliveryService.toString() );
		Object localAddr = null;
		try
		{
			localAddr = _deliveryService.transportQuery( Connection.LOCAL_ADDRESS );
		}
		catch (Exception e)
		{
		}
		if (localAddr != null)
		{
			_cachedLocalAddress = localAddr.toString();
		}
		Object remoteAddr = null;
		try
		{
			remoteAddr = _deliveryService.transportQuery( Connection.REMOTE_ADDRESS );
		}
		catch (Exception e)
		{
		}
		if (remoteAddr!=null)
		{
			_cachedRemoteAddress = remoteAddr.toString();
		}
		return String.format( "[Remote_Address:%s;Local_Address:%s]", _cachedRemoteAddress, _cachedLocalAddress );
	}

}
