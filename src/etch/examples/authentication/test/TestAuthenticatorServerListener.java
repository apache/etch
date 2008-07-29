/*
 * Created by rebarraz on Oct 25, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */
package etch.examples.authentication.test;

import metreos.core.io.Connection;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;

import org.junit.After;
import org.junit.Test;

import etch.examples.authentication.AuthDb;
import etch.examples.authentication.Authenticator;
import etch.examples.authentication.AuthenticatorHelper;
import etch.examples.configuration.Configuration;

/**
 * Simple tests to ensure that a listener of the proper handler can start and
 * stop successfully.
 *
 * @author rebarraz
 */
public class TestAuthenticatorServerListener
{
	// Time to wait between starting and stopping the listener.
	private static int iWaitPeriod = 4000;


	// Common Test Elements
	private AuthDb myAuthDb;
	private ListenerHandler myListenerHandler;
	private Connection<ListenerHandler> myListener;

	/**
	 * Stop the listener if it hasn't already been stopped.
	 *
	 * @throws Exception
	 */
	@After
	public void stopListener()
		throws Exception
	{
		if (myListener != null && myListener.isStarted())
			myListener.stop();
	}

	/**
	 * Start and stop a simple listener on an available port.
	 *
	 * @throws Exception
	 */
	@Test
	public void simpleServer()
		throws Exception
	{
		myAuthDb = new AuthDb();
		myListenerHandler = AuthenticatorHelper.newListenerHandler(myAuthDb);
		myListener = new Listener(myListenerHandler, 0, null, 0, 0);

		myListener.start();
		myListener.waitUp(iWaitPeriod);
	}

	/**
	 * Test listening locally with users.
	 *
	 * @throws Exception
	 */
	@Test
	public void localWithUsers()
		throws Exception
	{
		myAuthDb = new AuthDb();

		myAuthDb.addUser( "Fred", "cisco123" );
		myAuthDb.grantRight( "Fred", "Log.write" );

		myAuthDb.addUser( "Mary", "cisco456" );
		myAuthDb.grantRight( "Mary", Authenticator.ADMIN_RIGHT );
		myAuthDb.grantRight( "Mary", Authenticator.CHECK_RIGHT );

		myAuthDb.addUser( "Joe", "cisco789" );
		myAuthDb.grantRight( "Joe", Authenticator.CHECK_RIGHT );
		myAuthDb.grantRight( "Joe", Configuration.READ_RIGHT );

		myListenerHandler = AuthenticatorHelper.newListenerHandler(myAuthDb);
		myListener = new Listener(myListenerHandler, 5, "localhost", 0, 0);

		myListener.start();
		myListener.waitUp(iWaitPeriod);
	}
}
