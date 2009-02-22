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

package cisco.uc.cuae.etchrouter.test;

import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.PrintWriter;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import cisco.uc.cuae.etchrouter.EtchRouterMain;
import cisco.uc.cuae.etchrouter.test.app.MainTestPluginClient;
import cisco.uc.cuae.etchrouter.test.app.RemoteTestPluginServer;
import cisco.uc.cuae.etchrouter.test.app.TestPluginHelper;
import cisco.uc.cuae.etchrouter.test.utils.PluginTestServerRunner;

/**
 * JUnit test class for Etch Router
 * 
 * @author Wei Wang (weiwa@cisco.com)
 *
 */
public class TestEtchRouter
{

	private static String _routerUrlForClient = "tcp://127.0.0.1:9999/";
	
	private static EtchRouterMain _etchRouterRunner = null;
	
	private static PluginTestServerRunner _pluginRunner1 = null;
	
	private static PluginTestServerRunner _pluginRunner2 = null;
	
	/**
	 * 
	 * @throws Exception
	 */
	@BeforeClass
	public static void setUp() throws Exception
	{
		_pluginRunner1 = new PluginTestServerRunner("runner-4001", "tcp://0.0.0.0:4001" );
		_pluginRunner2 = new PluginTestServerRunner("runner-4002", "tcp://0.0.0.0:4002" );
		_pluginRunner1.start();
		_pluginRunner2.start();
		
		System.out.println("Sleeping 30 seconds after starting both plugin listeners...");
		Thread.sleep( 30000 );
		
		createPluginMeta( "roundrobin" );
		_etchRouterRunner = new EtchRouterMain( new File("EtchRouter-test.properties"), null);
		_etchRouterRunner.run();
		
		System.out.println("Sleeping 60 seconds after starting etch router...");
		Thread.sleep( 60000 );
	}

	private static void createPluginMeta( String pluginType ) throws Exception
	{
		File metaFile = new File("target/runtime/plugins/TestPlugin/metadata.txt");
		PrintWriter w = new PrintWriter( metaFile );
		try
		{
			w.println( String.format( "plugin.group.type=%s", pluginType ));
			w.println( "plugin.member.url.no1=tcp://127.0.0.1:4001" );
			w.println( "plugin.member.metadata.no1=location\\=LA" );
			w.println( "plugin.member.url.no2=tcp://127.0.0.1:4002" );
			w.println( "plugin.member.metadata.no2=location\\=LA" );
			w.println( "" );
		}
		finally
		{
			w.flush();
			w.close();
		}
		
	}
	
	/**
	 * 
	 * @throws Exception
	 */
	@AfterClass
	public static void tearDown() throws Exception
	{
		try
		{
			_pluginRunner1.stop();
		}
		catch (Exception e)
		{
			e.printStackTrace(System.out);
		}
		_etchRouterRunner.getRouterManager().stop();
	}

	@Test
	public void testRoundRobin() throws Exception
	{
		RemoteTestPluginServer server1 = TestPluginHelper.newServer( _routerUrlForClient, null,
			new MainTestPluginClient() );
		System.out.println("Starting remote server1...");
		server1._startAndWaitUp( 4000 );
		RemoteTestPluginServer server2 = TestPluginHelper.newServer( _routerUrlForClient, null,
			new MainTestPluginClient() );
		System.out.println("Starting remote server2...");
		server2._startAndWaitUp( 4000 );
		
		System.out.println("Sleep 30 seconds after starting both remote servers...");
		Thread.sleep( 30000 );

		int result1 = server1.addMethod( 11, 22 );
		assertTrue( String.format( "server1.addMethod(11,22) failed to return 33: %s", result1), result1==33 );
		assertTrue( "The call 'server1.addMethod(11,22)' was not routed to plugin listener1 ", _pluginRunner1.getNumCalls()==1);

		int result2 = server2.addMethod( 22, 33 );
		assertTrue( String.format( "server2.addMethod(22,33) failed to return 55: %s", result2), result2==55 );
		assertTrue( "The call 'server2.addMethod(22,33)' was not routed to plugin listener2 ", _pluginRunner2.getNumCalls()==1);
		
		//This method call doesn't really stop the listener
		//_pluginRunner2.stop();
		//System.out.println("Sleep 120 seconds for the plugin service to fail over...");
		//Thread.sleep( 120000 );
		
		//server2.addMethod( 1, 2 );
		//assertTrue( "After failover, the call 'server2.addMethod(1,2)' was not routed to plugin listener1 ", _pluginRunner1.getNumCalls()==2);
		
		System.out.println("Stopping remote server1...");
		server1._stopAndWaitDown( 4000 );
		
		System.out.println("Stopping remote server2...");
		server2._stopAndWaitDown( 4000 );
	}
}
