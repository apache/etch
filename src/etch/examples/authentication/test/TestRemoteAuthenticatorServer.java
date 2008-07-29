/*
 * Created by rebarraz on Oct 25, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */
package etch.examples.authentication.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;
import metreos.core.io.Connection;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import etch.bindings.java.support._Etch_AuthException;
import etch.examples.authentication.AuthDb;
import etch.examples.authentication.Authenticator;
import etch.examples.authentication.AuthenticatorHelper;
import etch.examples.authentication.AuthenticatorServer;
import etch.examples.authentication.RemoteAuthenticatorServer;
import etch.examples.authentication.Authenticator.Failure;
import etch.examples.configuration.Configuration;

/**
 * Simple tests to ensure that a listener of the proper handler can start and
 * stop successfully.
 *
 * @author rebarraz
 */
abstract public class TestRemoteAuthenticatorServer
{
	// Time to wait after server has started
	private static int iWaitPeriod = 4000;

	// Common test elements
	private String myHost;
	private int iMyPort;
	private int iMyDelay;
	private String myInstance;
	private AuthDb myAuthDb;
	private ListenerHandler myListenerHandler;
	private Connection<ListenerHandler> myListener;
	private AuthenticatorServer myAuthenticationServer;


	/**
	 * Reset common connection data for each test.
	 *
	 * @throws Exception
	 */
	@Before
	public void setupCommonData()
		throws Exception
	{
		myHost = "localhost";
		iMyPort = 4002;
		iMyDelay = 0;
		myInstance = AuthenticatorHelper.LOCAL;
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
		myListener = new Listener(myListenerHandler, 5, "localhost", iMyPort, iMyDelay);
		myListener.start();
		myListener.waitUp(iWaitPeriod);
	}


	/**
	 * Stop the server if needed.
	 *
	 * @throws Exception
	 */
	@After
	public void teardownServer()
		throws Exception
	{
		if (myListener != null && myListener.isStarted())
		{
			myListener.stop();
			myListener.waitDown( iWaitPeriod );
			myListener = null;
		}
		
		if (myAuthenticationServer != null)
		{
			((RemoteAuthenticatorServer) myAuthenticationServer)._stopAndWaitDown( 4000 );
			myAuthenticationServer = null;
		}
	}

	/**
	 * Call this method after you have instantiated and filled myUsers.  This
	 * will fill the AuthDb and start AuthenticationServer listening.
	 *
	 * @throws Exception
	 */
	private void startServer()
		throws Exception
	{
//		myListenerHandler = AuthenticatorHelper.newListenerHandler(myAuthDb);
//
//		myListener = new Listener(myListenerHandler, 3, myHost,
//			iMyPort, iMyDelay);
//
//		myListener.start();
//		myListener.waitUp(iWaitPeriod);

		myAuthenticationServer = AuthenticatorHelper.getAuthenticatorInstance(myHost,
			iMyPort, myInstance);
	}


	/**
	 * Start a server locally and see if the remote interface can find it.
	 *
	 * @throws Exception
	 */
	@Test
	public void simpleLocalServer()
		throws Exception
	{
		// No users needed for basic test. Clear them out.
//		myAuthDb = new AuthDb();

		startServer();
	}

	/**
	 * Test to ensure that random people cannot login.
	 *
	 * @throws Exception
	 */
	@Test
	(expected=Failure.class)
	public void noSuchAdmin()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Frederico", "p@55w0rD" );
	}

	/**
	 * Check that if an existing user with otherwise sufficient privleges tries
	 * to log in with an incorrect password, they will be unsuccessful.
	 *
	 * @throws Exception
	 */
	@Test
	(expected=Failure.class)
	public void badAdminPassword()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Mary", "p@55w0rD" );
	}

	/**
	 * Check that an existing user with insufficient privleges cannot log in.
	 *
	 * @throws Exception
	 */
	@Test
	(expected=Failure.class)
	public void insufficientAdminPrivleges()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Fred", "cisco123" );
	}

	/**
	 *  Tests to make sure a user can login successfully.
	 *
	 * @throws Exception
	 */
	@Test
	public void successfulAdminLogin()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Mary", "cisco456" );

		assertTrue(myAuthenticationServer.isLoggedIn());
	}

	/**
	 * Check to make sure no-such user can authenticate and that a failure is
	 * raised.
	 *
	 * @throws Exception
	 */
	@Test
	public void noSuchUserWCheckRights()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Joe", "cisco789" );

		assertFalse(myAuthenticationServer.isAuthentic( "Frederico", "p@55w0rD" ));
	}

	/**
	 * Check that if an existing user with otherwise sufficient privleges tries
	 * to log in with an incorrect password, they will be unsuccessful.
	 *
	 * @throws Exception
	 */
	@Test
	public void badUserPasswordWCheckRights()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Joe", "cisco789" );

		assertFalse(myAuthenticationServer.isAuthentic( "Mary", "p@55w0rD" ));
	}

	/**
	 *  Tests to make sure a user can login successfully.
	 *
	 * @throws Exception
	 */
	@Test
	public void successfulAuthenticationWCheckRights()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Joe", "cisco789" );

		assertTrue(myAuthenticationServer.isAuthentic("Fred", "cisco123" ));
		assertTrue(myAuthenticationServer.isAuthentic("Mary", "cisco456" ));
		assertTrue(myAuthenticationServer.isAuthentic("Joe", "cisco789" ));
	}

	/**
	 * Checks to see if the users have the expected rights.
	 *
	 * @throws Exception
	 */
	@Test
	public void checkForCorrectRightsWCheckRights()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Joe", "cisco789" );

		assertTrue(myAuthenticationServer.hasRight("Fred", "Log.write"));

		assertTrue(myAuthenticationServer.hasRight( "Mary", Authenticator.ADMIN_RIGHT));
		assertTrue(myAuthenticationServer.hasRight( "Mary", Authenticator.CHECK_RIGHT));

		assertTrue(myAuthenticationServer.hasRight( "Joe", Authenticator.CHECK_RIGHT));
		assertTrue(myAuthenticationServer.hasRight( "Joe", Configuration.READ_RIGHT));
	}

	/**
	 * Check to make sure no-such user can authenticate and that a failure is
	 * raised.
	 *
	 * @throws Exception
	 */
	@Test
	public void noSuchUserWAdminRights()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Mary", "cisco456" );

		assertFalse(myAuthenticationServer.isAuthentic( "Frederico", "p@55w0rD" ));
	}

	/**
	 * Check that if an existing user with otherwise sufficient privleges tries
	 * to log in with an incorrect password, they will be unsuccessful.
	 *
	 * @throws Exception
	 */
	@Test
	public void badUserPasswordWAdminRights()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Mary", "cisco456" );

		assertFalse(myAuthenticationServer.isAuthentic( "Mary", "p@55w0rD" ));
	}

	/**
	 *  Tests to make sure a user can login successfully.
	 *
	 * @throws Exception
	 */
	@Test
	public void successfulAuthenticationWAdminRights()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Mary", "cisco456" );

		assertTrue(myAuthenticationServer.isAuthentic("Fred", "cisco123" ));
		assertTrue(myAuthenticationServer.isAuthentic("Mary", "cisco456" ));
		assertTrue(myAuthenticationServer.isAuthentic("Joe", "cisco789" ));
	}

	/**
	 * Checks to see if the users have the expected rights.
	 *
	 * @throws Exception
	 */
	@Test
	public void checkForCorrectRightsWAdminRights()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Mary", "cisco456" );

		assertTrue(myAuthenticationServer.hasRight("Fred", "Log.write"));

		assertTrue(myAuthenticationServer.hasRight( "Mary", Authenticator.ADMIN_RIGHT));
		assertTrue(myAuthenticationServer.hasRight( "Mary", Authenticator.CHECK_RIGHT));

		assertTrue(myAuthenticationServer.hasRight( "Joe", Authenticator.CHECK_RIGHT));
		assertTrue(myAuthenticationServer.hasRight( "Joe", Configuration.READ_RIGHT));
	}

	/**
	 * Check that getRights is getting the correct rights.
	 *
	 *
	 * @throws Exception
	 */
	@Test
	public void adminCanGetRights()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Mary", "cisco456" );

		String[] tempRights = myAuthenticationServer.getRights("Mary");

		assertNotNull(tempRights);

		assertEquals(2, tempRights.length);

		assertNotNull(tempRights[0]);
		assertNotNull(tempRights[1]);

		assertTrue((tempRights[0].equals(Authenticator.ADMIN_RIGHT) &&
					tempRights[1].equals(Authenticator.CHECK_RIGHT)) ||
				   (tempRights[0].equals(Authenticator.CHECK_RIGHT) &&
				   	tempRights[1].equals(Authenticator.ADMIN_RIGHT)));

	}

	/**
	 * Make sure a non-admin can't getRights.
	 *
	 * @throws Exception
	 */
	@Test
	(expected=_Etch_AuthException.class)
	public void nonAdminFailsGetRights()
		throws Exception
	{
		startServer();

		myAuthenticationServer.login( "Joe", "cisco789" );

		myAuthenticationServer.getRights("Joe");
	}
}
