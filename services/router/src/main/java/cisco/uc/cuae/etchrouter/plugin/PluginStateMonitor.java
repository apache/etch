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
package cisco.uc.cuae.etchrouter.plugin;

import java.util.Collection;
import java.util.logging.Level;
import java.util.logging.Logger;

import cisco.uc.cuae.etchrouter.EtchRouterManager;

/**
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class PluginStateMonitor implements Runnable
{
	private static final Logger _LOGGER = Logger.getLogger( PluginStateMonitor.class.getName());
	
	private int _sleepTime = 60000;
	
	private EtchRouterManager _etchRouterManager = null;
	
	private boolean _running = true;
	
	/**
	 * 
	 * @param etchRouterManager
	 */
	public PluginStateMonitor(EtchRouterManager etchRouterManager)
	{
		_etchRouterManager = etchRouterManager;
		try
		{
			_sleepTime = Integer.parseInt( _etchRouterManager.getProperty( "plugin.monitor.sleep.interval", "" ) );
		}
		catch (Exception e)
		{
		}
	}
	
	/**
	 * 
	 */
	public void stop()
	{
		_running = false;
	}
	
	public void run()
	{
		int sleepBetween = 2*_etchRouterManager.getConnectionStartMaxDelay();
		while (_running)
		{
			_LOGGER.log( Level.FINE, "Checking each plugin member's state..." );
			Collection<PluginGroup> pluginGrps = _etchRouterManager.getPluginGroups();
			for (PluginGroup pluginGrp : pluginGrps)
			{
				Collection<PluginMember> members = pluginGrp.getPluginMembers();
				for (PluginMember member : members)
				{
					Thread runner = new Thread(new CheckPluginMemberStateRunner(member));
					runner.start();
				}
			}
			
			_LOGGER.log( Level.FINE, "Sleeping for {0} miliseconds before refreshing applications...", sleepBetween );
			try
			{
				Thread.sleep( sleepBetween );
			}
			catch (Exception e)
			{
				
			}
			
			_LOGGER.log( Level.FINE, "Refreshing each application's connection..." );
			for (PluginGroup pluginGrp : pluginGrps)
			{
				pluginGrp.refreshAppConnections();
			}
			
			_LOGGER.log( Level.FINE, "Sleeping for {0} miliseconds before starting next cycle...", _sleepTime );
			try
			{
				Thread.sleep( _sleepTime );
			}
			catch (Exception e)
			{
			}
		}
	}
	
	private class CheckPluginMemberStateRunner implements Runnable
	{
		private PluginMember _pluginMember = null;
		
		public CheckPluginMemberStateRunner(PluginMember member)
		{
			_pluginMember = member;
		}
		
		public void run()
		{
			_pluginMember.checkState( _etchRouterManager );
		}
	}

}
