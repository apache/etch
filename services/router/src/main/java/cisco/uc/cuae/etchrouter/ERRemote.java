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

import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.DeliveryService;
import etch.bindings.java.support.RemoteBase;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class ERRemote extends RemoteBase
{
	
	private EtchRouterManager _etchRouterManager = null;
	
	private ConnectionStackInfo _connectionInfo = null;
	
	/**
	 * 
	 * @param svc
	 * @param vf
	 * @param etchRouterManager
	 */
	public ERRemote( DeliveryService svc, ValueFactory vf, EtchRouterManager etchRouterManager, ConnectionStackInfo connInfo )
	{
		super( svc, vf );
		_etchRouterManager = etchRouterManager;
		_connectionInfo = connInfo;
	}
	
	public EtchRouterManager getEtchRouterManager()
	{
		return _etchRouterManager;
	}
	
	public ConnectionStackInfo getConnectionStackInfo()
	{
		return _connectionInfo;
	}

	@Override
	public String toString()
	{
		return String.format( "%s[connection-info:%s]", this.getClass().getSimpleName(), _connectionInfo );
	}
	
	
}
