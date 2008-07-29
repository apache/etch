/*
 * Created by rebarraz on Oct 25, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */
package etch.examples.locator.test;

import java.util.ArrayList;
import java.util.List;

import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;

import org.junit.After;
import org.junit.Before;
import org.junit.BeforeClass;
import org.junit.Test;

import etch.examples.locator.LocatorServer;
import etch.examples.locator.Locator.Registration;

/**
 * Simple test for a remote LocatorServer.
 *
 * @author rebarraz
 */
public class TestRemoteLocatorServer
{
	// --------
	// Settings
	// --------

	private static final String host = "localhost";

	private static final int port = 4001;

	private static List<Registration> contexts;

	private static ListenerHandler lh;

	private static Listener l;

	private static LocatorServer remoteServer;


	// --------
	// Up, Down
	// --------

	/**
	 * Start the remote LocatorServer.
	 *
	 * @throws Exception
	 */
	@BeforeClass
	public static void setupRemoteLocatorServer()
		throws Exception
	{
		contexts = new ArrayList<Registration>();

		// TODO: add some registration data once Locator is stable.

		// lh = LocatorHelper.newListenerHandler(contexts);

		// l = new Listener(lh, 5, null, port, 0);
	}


	/**
	 * Contact the remote server.
	 *
	 *@throws Exception
	 */
	@Before
	public void getRemoteServer()
		throws Exception
	{
		// remoteServer = LocatorHelper.getLocator(host, port);
	}


	/**
	 * Forget the remote server
	 *
	 *@throws Exception
	 */
	@After
	public void forgetRemoteServer()
		throws Exception
	{
		// remoteServer = null;
	}


	// -------
	// Testing
	// -------

	/**
	 * TODO: Finish up Locator and test a remote server.  This is just here to keep
	 * JUnit from whining.
	 */
	@Test
	public void implementRemoteLocatorServerTests()
	{
		// nada
	}
}
